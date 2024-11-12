#pragma once
#include "Block.h"
class Blockchain
{
public:
	Blockchain();
	~Blockchain() {};

	// Get the latest Block in the chain
	Block getLatestBlock() const;

	// Method to mine pending transactions and add a new block to the chain
	void minePendingTransaction(const std::string& minerAddress);
	void displayChain() const;

	// Method to validate the chain
	bool isChainValid() const;

private:
	std::vector<Block> _chain;
	std::vector<Transaction> _pendingTransactions;

	Block createGenesisBlock();
};

