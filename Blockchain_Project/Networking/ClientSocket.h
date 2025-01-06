#pragma once
#include "Socket.h"

class ClientSocket
{
public:
	ClientSocket(int destPort);
	std::string sendAndRecv(std::string message) const;
	void sendMsg(std::string message) const;
private:
	SOCKET clientSocket_;
};
