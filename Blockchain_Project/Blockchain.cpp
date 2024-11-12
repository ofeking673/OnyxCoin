#include "Blockchain.h"

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

void Blockchain::minePendingTransaction(const std::string& minerAddress)
{
	// TODO: Consensus PBFT

	Block newBlock(_chain.size(), getLatestBlock().getHash());
	for (const auto& tx : _pendingTransactions)
	{
		newBlock.addTransaction(tx);
	}

	// TODO: Reward?
	Transaction rewardTX("System", minerAddress, 100);
	newBlock.addTransaction(rewardTX);
	// ?

	// Calculate and set the hash of the new block
	newBlock.setHash(newBlock.calculateHash());

	_chain.push_back(newBlock);
	_pendingTransactions.clear();
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
