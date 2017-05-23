#include "luasandboxserver.h"

LuaSandboxServer::LuaSandboxServer() {
	server.clear_error_channels(websocketpp::log::alevel::all);
	server.clear_access_channels(websocketpp::log::alevel::all);

	server.init_asio();
	server.set_reuse_addr(true);
	
	server.set_message_handler(websocketpp::lib::bind(
		&LuaSandboxServer::onMessage, this, websocketpp::lib::placeholders::_1,
		websocketpp::lib::placeholders::_2));
}

bool LuaSandboxServer::validateMessage(Server::message_ptr msg) {
	return true;
}

void LuaSandboxServer::onMessage(Connection con, Server::message_ptr msg) {
	if (!validateMessage(msg)) return;
	{
		LockGuard guard(requestLock);
		queue.emplace(con, msg->get_payload());
	}
	requestCondition.notify_one();
}

void LuaSandboxServer::processQueue(int threadId) {
	LuaSandbox sandbox;

	while (1) {
		UniqueLock lock(requestLock);
		while (queue.empty()) {
			requestCondition.wait(lock);
		}

		std::pair<Connection, std::string> request = queue.front();
		queue.pop();
		lock.unlock();

		try {
			Server::connection_ptr conptr = server.get_con_from_hdl(request.first);
			if (conptr) {
				{
					LockGuard guard(sendLock);
					server.ping(request.first, "are you alive?");
					std::cout << "Thread #" << threadId << " executing code." << std::endl;
				}

				std::string result = sandbox.runScriptSafely(request.second, 0);

				LockGuard guard(sendLock);
				server.send(request.first, result.c_str(), websocketpp::frame::opcode::text);
			}
		} catch (...) {

		}
	}
}

void LuaSandboxServer::run(uint16_t port) {
	server.listen(port);
	server.start_accept();
	server.run();
}
