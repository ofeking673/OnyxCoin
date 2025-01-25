#include "Socket.h"
#include "../Server.h"

Socket::Socket(int destPort)
{
	
	auto serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("socket() failed");
		system("pause");
	}	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(destPort);
	serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	serverSocket_ = serverSocket;
}

void Socket::WaitForClients(void(*func)(SOCKET), Server* serv) const
{
	listen(serverSocket_, 5);
	while (true)
	{
		auto clientSocket = accept(serverSocket_, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << __FUNCTION__ ": Error in accept: " << WSAGetLastError() << std::endl;
		}
		else {
			std::cout << __FUNCTION__ ": Client connected!" << std::endl;
			if (serv) {
				std::thread client(&Server::HandleClient, serv, clientSocket);
				client.detach();
			}
			else {
				std::thread client(func, clientSocket);
				client.detach();
			}
		}
	}
}

void Socket::sendMessage(SOCKET sock, std::string& msg)
{
	send(sock, msg.c_str(), msg.size(), 0);
}

std::string Socket::readFromSock(SOCKET sock)
{
	char buf[READ_SIZE];
	int amt = recv(sock, buf, READ_SIZE, 0);
	if (amt > 0) {
		std::cout << amt;
		buf[amt+1] = '\0';
		std::string a(buf, amt);
		//std::cout << __FUNCTION__ ": " << a << std::endl;
		return a;
	}
	else {
		throw std::runtime_error("No message recieved! " __FUNCTION__);
	}
	// use __FUNCTION__ for logging! For this implementation we use one project for users, miners, and server.
}
