#pragma once
#include "IClient.h"
#include "Networking/ClientSocket.h"

class Client : public IClient
{
public:
	Client(int port, std::string keyPath, int srcPort = 0) : IClient(keyPath, port, srcPort) {}
	std::string signMessage(std::string msg);
};

