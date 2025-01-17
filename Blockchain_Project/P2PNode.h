#pragma once
#include "Server.h"
#include "Miner.h"
#include "Client.h"

// Discovery server is localhost:4444
// Will open this port if we decide to goo full p2p support for nodes outside of subnet
#define DISCOVERY 4444 

class P2PNode
{
public:
	P2PNode(int handlerIdentifier, std::string keyPath, int port);
	~P2PNode();

	void pingAll();
private:
	std::string signMessage(std::string message);

	IMessageHandler* _handler;
	MessageManager _messageCreator;
};

