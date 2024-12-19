#pragma once
#include "IClient.h"
#include "Blockchain.h"
#include "Networking/ClientSocket.h"
#include "Encryptions/SHA256.h"

class Miner : public IClient
{
public:
	Miner(std::string key, int port) : IClient(key, port) {}
	void mine();
};

