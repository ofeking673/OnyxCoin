#pragma once
#include "Server.h"
#include "Miner.h"
#include "Client.h"
#include "PeerManager.h"

// Discovery server is localhost:4444
// Will open this port if we decide to goo full p2p support for nodes outside of subnet
#define DISCOVERY 4444 


class P2PNode
{
public:
	P2PNode(std::string keyPath) : handler(keyPath) {};
private:
	FullNodeMessageHandler handler;
};

