#pragma once
#include "Socket.h"

class ClientSocket
{
public:
	ClientSocket(std::string ip, int destPort);
	std::string sendAndRecv(std::string message) const;
	void sendMsg(std::string message) const;
	std::string recvMsg() const;

	// For peerManager utility
	SOCKET getSocket() const { return clientSocket_; };
private:
	SOCKET clientSocket_;
	std::pair<std::string, int> _personalSocketDetails;
};
