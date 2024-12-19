#pragma once
#include "Block.h"

class Blockchain
{
public:
	~Blockchain();
	static Blockchain* getInstance() 
	{
		if (!_instance) 
		{ 
			_instance = new Blockchain();
		} 
		return _instance; 
	}

	// Get the latest Block in the chain
	Block getLatestBlock() const;

	void addTransaction(const Transaction& tx);

	// Method to mine pending transactions and add a new block to the chain
	//void minePendingTransaction(const std::string& minerAddress);
	bool submitMiningHash(const std::string address, std::string finalHash, int nonce);
	std::string getCurrentBlockInfo(); //For mining purposes, need to hash this and find the correct nonce.
	void displayBlockchain() const;

	// Method to validate the chain
	bool isChainValid() const;
	const std::vector<Block> getChain() { return _chain; }; //this is not a pointer nor a reference to avoid changing the chain, Thus this is a read only chain.
	static SHA256* sha;
private:
	Blockchain();
	static Blockchain* _instance;
	std::vector<Block> _chain;
	std::vector<Transaction> _pendingTransactions;
	Block createGenesisBlock();

};

