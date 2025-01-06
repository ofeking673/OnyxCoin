#pragma once
#include "IClient.h"
#include "Networking/ClientSocket.h"

class Client : public IClient
{
public:
	Client(int port, std::string keyPath) : IClient(keyPath, port) {}
	void initializeTransaction(std::string dstAddress, double amt);
	
};

