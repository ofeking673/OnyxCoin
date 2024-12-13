#pragma once
#include "IClient.h"
#include "Blockchain.h"
#include "Networking/ClientSocket.h"
#include "Encryptions/SHA256.h"

class Miner : public IClient
{
public:
	Miner(std::string key) { k = key; };
	virtual void Action() override;
	static SHA256* sha;
};

