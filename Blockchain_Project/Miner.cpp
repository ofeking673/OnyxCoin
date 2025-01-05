#include "Miner.h"


void Miner::mine()
{
	Blockchain* chain = Blockchain::getInstance();
	//placeholder mining algorithm, will probably be configured and changed later.
	int nonce = -1;
	std::string hash;
	std::string data = chain->getCurrentBlockInfo(pubKey);
	std::cout << data << std::endl;
	std::cout << __FUNCTION__ ": Currently mining for hash starting with '0'...\n";
	do {
		nonce++;
		hash = SHA256::digest(data + std::to_string(nonce));
		std::cout << __FUNCTION__ ": Current hash: " << hash << std::endl;
	} while (!hash.starts_with('0'));
	std::cout << __FUNCTION__ ": Found correct hash! " << hash << std::endl;
	std::string ser = JsonPacketSerializer::serializeMiningRequest(_k, hash, nonce);
	
	std::string res = sock.sendAndRecv(ser);
	std::cout << __FUNCTION__ ": Got response for mining request: " << res << std::endl;
}
