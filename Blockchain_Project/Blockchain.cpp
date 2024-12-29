#include "Blockchain.h"

Blockchain* Blockchain::_instance = nullptr;
Blockchain::Blockchain()
{
	// Create the genesis block
	Block genesisBlock = createGenesisBlock();
	_chain.push_back(genesisBlock);
}

Blockchain::~Blockchain()
{
	_chain.clear();
	_pendingTransactions.clear();
}

Block Blockchain::getLatestBlock() const
{
	return _chain.back();
}

void Blockchain::addTransaction(const Transaction& tx)
{
	_pendingTransactions.push_back(tx);
}

bool Blockchain::submitMiningHash(const std::string address, std::string finalHash, int nonce)
{
	std::string hash = getCurrentBlockInfo() + std::to_string(nonce);
	hash = SHA256::digest(hash);
	
	if (hash.starts_with('0') && hash == finalHash) {
		/*Transaction reward("System", address, 10);
		_pendingTransactions.clear();
		_pendingTransactions.push_back(reward);
		return true;*/
	}
	return false;
}

std::string Blockchain::getCurrentBlockInfo()
{
	Block newBlock(_chain.size(), getLatestBlock().getHash());
	for (const auto& tx : _pendingTransactions)
	{
		newBlock.addTransaction(tx);
	}

	return newBlock.getCurrentBlockInfo();
}

void Blockchain::displayBlockchain() const
{
	for (const auto& block : _chain)
	{
		block.displayBlock();
		std::cout << "=========================" << std::endl;
	}
}

bool Blockchain::isChainValid() const
{
	for (int i = 1; i < _chain.size(); i++)
	{
		const Block& current = _chain[i];
		const Block& previous = _chain[i - 1];

		if (current.getHash() != current.calculateHash())
		{
			return false;
		}

		if (current.getPreviousHash() != previous.getHash())
		{
			return false;
		}
	}
	return true;
}

Block Blockchain::createGenesisBlock()
{
	Block genesis(0, "0");
	return genesis;
}
