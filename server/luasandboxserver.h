#ifndef LUASANDBOXSERVER_H
#define LUASANDBOXSERVER_H

#include <queue>
#include <vector>
#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include "luasandbox.h"

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

		LuaSandboxServer();

		bool validateMessage(Server::message_ptr msg);
		void onMessage(Connection con, Server::message_ptr msg);
		void processQueue(int threadId);
		void run(uint16_t port);

	private:
		Server server;
		RequestQueue queue;
		Mutex requestLock;
		Mutex sendLock;
		ConditionVariable requestCondition;
};

#endif
