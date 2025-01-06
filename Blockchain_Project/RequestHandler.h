#pragma once
#include "..\Blockchain_Project\Networking\Socket.h"
#include "Client.h"
#include "Miner.h"
#include "UTXOSet.h"
class RequestHandler
{
public:
	static RequestHandler* getInstance() 
	{ 
		if (!_instance) 
		{ 
			_instance = new RequestHandler(); 
		} 
		return _instance;
	};
	static std::string mine(std::string& address, json j);
	static std::string transaction(std::string& address, json j);
private:
	RequestHandler();
	static RequestHandler* _instance;
	static Blockchain* blockchain;
	static UTXOSet* utxo;
};

