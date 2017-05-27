#include <iostream>

#include <boost/program_options.hpp>

#include "luasandboxserver.h"

int main(int argc, const char *argv[]) {
	int threads;
	int port;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("threads", boost::program_options::value<int>(&threads)->default_value(5), "number of worker threads")
	    ("port", boost::program_options::value<int>(&port)->default_value(6000), "server port");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) {
	    std::cout << desc << std::endl;
	    return 1;
	}

	if (threads <= 0) threads = 1;

	std::cout << "Server running on port " << port << " with " << threads << " worker threads." << std::endl;

	LuaSandboxServer server;

	std::vector<LuaSandboxServer::Thread> workerThreads;
	for (int i = 0; i < threads; i++) {
		workerThreads.emplace_back(websocketpp::lib::bind(&LuaSandboxServer::processQueue, &server, i));
	}

	server.run(port);

	for (auto& worker : workerThreads) {
		worker.join();
	}

	return 0;
}
