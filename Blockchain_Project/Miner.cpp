#include "Miner.h"
SHA256* Miner::sha = new SHA256();


void Miner::Action()
{
	Blockchain* chain = Blockchain::getInstance();
	//placeholder mining algorithm, will probably be configured and changed later.
	int nonce = 0;
	std::string hash;
	std::string data = chain->getCurrentBlockInfo();
	std::cout << __FUNCTION__ ": Currently mining for hash starting with '0'...\n";
	while (!hash.starts_with('0')) { //a 1/16 chance!
		std::cout << __FUNCTION__ ": Current hash: " << hash << std::endl;
		hash = sha->digest(data + std::to_string(nonce));
	}
	std::cout << __FUNCTION__ ": Found correct hash! " << hash << std::endl;
	chain->addTransaction(Transaction("System", k, 10));
}
