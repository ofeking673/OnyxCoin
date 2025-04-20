#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "Networking/WSAInitializer.h"
#include "DiscoveryServer.h"
#include "P2PNode.h"
#include "FullNode.h"

#include "IMessageHandler.h"
DiscoveryServer* createDiscoveryServer();

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage
	/// Initializing the basic discovery server
	DiscoveryServer* disc = createDiscoveryServer();
	disc->start("127.0.0.1", 4444);

	try {
		// Initialize the FullNode with a wallet file path, listen address, and port.

		std::cout << "Enter <filename> <listening port>" << std::endl;
		std::string line;
		std::getline(std::cin, line);
		uint16_t port;
		std::string filename;
		std::istringstream iss(line);

		iss >> filename >>port;

		FullNode node1(filename, "127.0.0.1", port);

		while (node1.isRunning())
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		system("pause");

	}
	catch (const std::exception& e) {
		std::cerr << "[Exception] " << e.what() << std::endl;
	}

	system("pause");
	return 0;
}

DiscoveryServer* createDiscoveryServer() {

	return new DiscoveryServer("../discovery");
}