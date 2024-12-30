#include "RequestHandler.h"
Blockchain* RequestHandler::blockchain = nullptr;
UTXOSet * RequestHandler::utxo = nullptr;

RequestHandler::RequestHandler()
{
	blockchain = Blockchain::getInstance();
	utxo = UTXOSet::getInstance();
}

std::string RequestHandler::mine(std::string& address, json j)
{
	//Get the nonce and new hash from sock
	auto info = blockchain->getCurrentBlockInfo();
	std::string expectedHash = j["hash"];
	int nonce = j["nonce"];
	std::string hash = SHA256::digest(info + std::to_string(nonce));
	if (hash == expectedHash) {
		//nonce is correct, mining is solved
		//blockchain->addTransaction(Transaction("System", address, 10)); //reward user on mining
	}
	std::string response = JsonPacketSerializer::serializeMiningResponse((hash == expectedHash), 1);
	return response;
}

std::string RequestHandler::transaction(std::string& address, json j)
{
	
}
