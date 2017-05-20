#include <queue>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

class LuaSandBox {
	public:
		LuaSandBox() {
			L = luaL_newstate();
			if (!L) {
				throw std::runtime_error("Couldn't allocate enough memory for a new lua state.");
			}

			luaL_openlibs(L);
			// Load lua library functions
			luaL_dofile(L, "./library.lua");
		}

		std::string runScript(std::string chunk) {
			std::string ret;

			// Call setupSafeEnvironment
			lua_getglobal(L, "setupSafeEnvironment");
			if (!lua_isfunction(L, -1)) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}
			if (lua_pcall(L, 0, 0, 0) != 0) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}

			// Call setTimeout
			lua_getglobal(L, "setTimeout");
			if (!lua_isfunction(L, -1)) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}
			lua_pushinteger(L, 15);
			if (lua_pcall(L, 1, 0, 0) != 0) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}

			// Load setfenv to use in loaded function
			lua_getglobal(L, "setfenv");
			if (!lua_isfunction(L, -1)) {
				lua_pop(L, 1);
				return "Internal Error: couldn't run this script.";
			}

			if (luaL_loadstring(L, chunk.c_str()) != 0) {
				// Error while loading compiling chunk
				ret = lua_tostring(L, -1);
				lua_pop(L, 2);
			} else {
				lua_getglobal(L, "LuaSandboxEnvironment");
				lua_call(L, 2, 1);

				int base = lua_gettop(L);
				lua_pushcfunction(L, traceback);
				lua_insert(L, base);
				
				if (lua_pcall(L, 0, 0, base) != 0) {
					ret = lua_tostring(L, -1);
				} else {
					lua_getglobal(L, "LuaSandboxEnvironment");
					if (!lua_istable(L, -1)) {
						lua_pop(L, 1);
						return "Internal Error: couldn't run this script.";
					}
					lua_pushstring(L, "_outputMessage");
					lua_gettable(L, -2);
					if (lua_isstring(L, -1)) {
						ret = lua_tostring(L, -1);
					}
					lua_pop(L, 1);
				}

				lua_pop(L, 1);
				lua_remove(L, base);
			}

			return ret;
		}

		~LuaSandBox() {
			lua_close(L);
		}

	private:
		lua_State* L;

		static int traceback (lua_State *L) {
			if (!lua_isstring(L, 1)) return 1;

			lua_getglobal(L, "debug");
			if (!lua_istable(L, -1)) {
		    	lua_pop(L, 1);
		    	return 1;
		  	}
			lua_getfield(L, -1, "traceback");
			if (!lua_isfunction(L, -1)) {
				lua_pop(L, 2);
				return 1;
		  	}
		  	lua_pushvalue(L, 1);  /* pass error message */
		  	lua_pushinteger(L, 2);  /* skip this function and traceback */
		  	lua_call(L, 2, 1);  /* call debug.traceback */
		  	return 1;
		}
};

class LuaSandboxServer {
	public:
		typedef websocketpp::server<websocketpp::config::asio> Server;
		typedef websocketpp::connection_hdl Connection;
		typedef std::queue<std::pair<Connection, std::string>> RequestQueue;
		typedef websocketpp::lib::thread Thread;
		typedef websocketpp::lib::mutex Mutex;
		typedef websocketpp::lib::lock_guard<Mutex> LockGuard;
		typedef websocketpp::lib::unique_lock<Mutex> UniqueLock;
		typedef websocketpp::lib::condition_variable ConditionVariable;

		LuaSandboxServer() {
			server.clear_error_channels(websocketpp::log::alevel::all);
			server.clear_access_channels(websocketpp::log::alevel::all);

			server.init_asio();
			server.set_reuse_addr(true);
			
			server.set_message_handler(websocketpp::lib::bind(
				&LuaSandboxServer::onMessage, this, websocketpp::lib::placeholders::_1,
				websocketpp::lib::placeholders::_2));
		}

		bool validateMessage(Server::message_ptr msg) {
			return true;
		}

		void onMessage(Connection con, Server::message_ptr msg) {
			if (!validateMessage(msg)) return;

			{
				LockGuard guard(requestLock);
				queue.emplace(con, msg->get_payload());
			}
			requestCondition.notify_one();
		}

		void processQueue(int threadId) {
			LuaSandBox sandbox;

			while (1) {
				UniqueLock lock(requestLock);
				while (queue.empty()) {
					requestCondition.wait(lock);
				}

				std::pair<Connection, std::string> request = queue.front();
				queue.pop();
				lock.unlock();

				std::cout << "Thread #" << threadId << ": executing code." << std::endl;
				Server::connection_ptr conptr = server.get_con_from_hdl(request.first);
				if (conptr) {
					try {
						std::string result = sandbox.runScript(request.second);
						server.send(request.first, result.c_str(), websocketpp::frame::opcode::text);
					} catch (...) {

					}
				}
			}
		}

		void run(uint16_t port) {
			server.listen(port);
			server.start_accept();
			server.run();
		}

	private:
		Server server;
		RequestQueue queue;
		Mutex requestLock;
		ConditionVariable requestCondition;


};

int main(int argc, const char *argv[]) {
	LuaSandboxServer server;
	
	std::vector<LuaSandboxServer::Thread> workerThreads;
	for (int i = 0; i < 5; i++) {
		LuaSandboxServer::Thread worker(websocketpp::lib::bind(&LuaSandboxServer::processQueue, &server, i));
		workerThreads.emplace_back(std::move(worker));
	}

	server.run(6000);

	for (auto& worker : workerThreads) {
		worker.join();
	}
}
