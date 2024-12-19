#include "ClientSocket.h"

ClientSocket::ClientSocket(int destPort)
{
	auto clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(destPort);
	serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	clientSocket_ = clientSocket;
}

std::string ClientSocket::sendAndRecv(std::string message) const
{
	char buf[READ_SIZE];
	send(clientSocket_, message.c_str(), message.size(), 0);
	int bytesRecieved = recv(clientSocket_, buf, READ_SIZE, 0);
	if (bytesRecieved > 0)
	{
		return std::string(buf, bytesRecieved);
	}
	else
	{
		throw std::runtime_error("Recv resulted in nothing. Message: " + message + "\n\n");
	}
}

void ClientSocket::sendMsg(std::string message) const
{
	send(clientSocket_, message.c_str(), message.size(), 0);
}


