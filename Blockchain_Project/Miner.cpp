#include "Miner.h"


void Miner::mine()
{
	Blockchain* chain = Blockchain::getInstance();
	//placeholder mining algorithm, will probably be configured and changed later.
	int nonce = 0;
	std::string hash;
	std::string data = chain->getCurrentBlockInfo();
	std::cout << __FUNCTION__ ": Currently mining for hash starting with '0'...\n";
	while (!hash.starts_with('0')) { //a 1/16 chance
		std::cout << __FUNCTION__ ": Current hash: " << hash << std::endl;
		hash = Blockchain::sha->digest(data + std::to_string(nonce));
	}
	std::cout << __FUNCTION__ ": Found correct hash! " << hash << std::endl;
	std::string ser = JsonPacketSerializer::serializeMiningRequest(this->_k, hash, nonce);
	
	std::string res = sock.sendAndRecv(ser);
	std::cout << __FUNCTION__ ": Got response for mining request: " << res << std::endl;
}
