#include "pch.h"

#include "Networking/FullNode.h"

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage

	try {
		// Initialize the FullNode with a wallet file path, listen address, and port.

		std::cout << "Enter <filename> <listening port>" << std::endl;
		std::string line;
		std::getline(std::cin, line);
		uint16_t port;
		std::string filename;
		std::istringstream iss(line);

		iss >> filename >> port;

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
