#pragma once
#include "IClient.h"
#include "Networking/ClientSocket.h"

class Client : public IClient
{
	Client(int port, std::string key) : IClient(key, port) {}
	void initializeTransaction(std::string dstAddress, double amt);
	
};

