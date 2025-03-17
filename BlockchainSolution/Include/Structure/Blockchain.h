#pragma once
#include "Block.h"
#include "Wallets/UTXOSet.h"
#include "Wallets/Mempool.h"

class Blockchain
{
public:
	// Create with genesis block
	Blockchain();
	// Create without genesis block
	Blockchain(int);
	~Blockchain();

	// Get the latest Block in the chain
	Block getLatestBlock() const;

	std::string getChain() const;

	bool isBlockValid(const Block& block) const;

	bool addBlock(const Block& block);
	// There are awaited headers. Recieved a block, than add it if available to the first wawaited header.
	bool addFullBlockToFirstAwaitedHeader(const Block& block);

	void addTransaction(const Transaction& tx);

	bool isAvailableToCommitBlock();
	void displayBlockchain() const;
	Block commitBlock(std::string leadersPublicKey);
	void addBlockToUtxo(Block& block); //Iterate over all transactions and add the outputs to the UTXO set
	// Method to validate the chain
	bool isChainValid() const;
	const std::vector<Block> getChain() { return _chain; }; //this is not a pointer nor a reference to avoid changing the chain, Thus this is a read only chain.
	// Get pending transactions
	std::vector<Transaction> getPendingTransactions() const;

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
	

	bool isUTXOLocked(const OutPoint& op) const;

	// Mining
	// Mine a new block. Find a correct nonce to produce hash starts with 0s
	void mineNewProposedBlock(Block& proposedBlock, const std::string& minerPublicKey);

	// Function to generate and return a random uint64_t.
	uint64_t getRandom();
private:
	Transaction testTransaction(std::string address, uint64_t amt);
	void addRewardTransaction(const std::string& address, Block& newBlock);

	std::vector<Block> _chain;
	
	//std::vector<Transaction> _pendingTransactions;
	Mempool mempool;

	UTXOSet* utxo;
	Block createGenesisBlock();

	// Random
	std::mt19937_64 gen;                             // 64-bit Mersenne Twister engine.
	std::uniform_int_distribution<uint64_t> dis;     // Uniform distribution for uint64_t.
};

