#pragma once
#include "Block.h"
#include "UTXOSet.h"

class Blockchain
{
public:
	Blockchain();
	~Blockchain();

	///////////////////////////////////////////////////////////
	//FOR TESTING ONLY!! REMOVE LATER!
	void testTransaction(std::string address, uint64_t amt);
	///////////////////////////////////////////////////////////
	// Get the latest Block in the chain
	Block getLatestBlock() const;

	bool isBlockValid(const Block& block) const;

	void addBlock(const Block& block);

	void addTransaction(const Transaction& tx);

	void displayBlockchain() const;
	void commitBlock();
	void addBlockToUtxo(Block block); //Iterate over all transactions and add the outputs to the UTXO set
	// Method to validate the chain
	bool isChainValid() const;
	const std::vector<Block> getChain() { return _chain; }; //this is not a pointer nor a reference to avoid changing the chain, Thus this is a read only chain.

	const Transaction findTransactionInPending(const std::string& txID) const;
	const Transaction findTransactionInChain(const std::string& txID) const;
	const Block findBlock(const std::string& blockHash, const std::string& prevBlockHash) const;
	const BlockHeader findHeader(const std::string& blockHash, const std::string& prevBlockHash) const;

	bool hasTransaction(const std::string& txID) const;
	bool hasBlock(const std::string& blockHash, const std::string& prevBlockHash) const;

	void appendHeaders(const std::vector<BlockHeader>& newHeaders);
	int getHeightByHash(const std::string& hash) const;
	std::vector<BlockHeader> getHeadersFrom(int startIndex, int maxCount, const std::string& stopHash = "") const;
	std::vector<BlockHeader> getAppendedHeaders() const;

private:
	std::vector<Block> _chain;
	std::vector<Transaction> _pendingTransactions;
	UTXOSet* utxo;
	Block createGenesisBlock();

};

