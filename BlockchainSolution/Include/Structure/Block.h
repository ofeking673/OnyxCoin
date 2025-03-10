#pragma once
#include "Wallets/Transaction.h"
#include "BlockHeader.h"
#include <vector>
#include <sstream>



#define ERROR_BLOCK_HASH -1
class Block
{
public:
	Block(int index, const std::string& previousHash);
	Block(); // Error
	Block(BlockHeader blockHeader);
	~Block();

	bool isErrorBlock() const;
	void addTransaction(const Transaction& transaction);
	std::string calculateHash() const;
	std::string getCurrentBlockInfo() const; //Mining purposes

	void displayBlock() const;
	std::string toMessageString() const;
	static Block parseMessageString(const std::string& data);
	nlohmann::json toJson() const;
	static Block fromJson(nlohmann::json data);

	std::string getHash() const;
	std::string getPreviousHash() const;
	void setHash(const std::string& hash);

	void setNonce(const uint64_t& nonce);

	const BlockHeader getBlockHeader() const;

	const Transaction findTransaction(const std::string& txID) const;

	// Calculate the reward for the leader, for proposing the block.
	// Call before adding the reward transaction.
	uint64_t calculateBlockReward();


	// Check if the calculated hash is ok and starts with zeros
	bool checkHash(const uint64_t& nonce);
	// Check if the other block is the mined version of the block
	bool checkIfBlockMined(const Block& minedBlock) const;

	std::vector<Transaction> _transactions; // Make transactions public for later verifications
private:
	//int _index;
	//time_t _timestamp;
	//std::string _previousHash;
	//std::string _hash;
	BlockHeader _blockHeader;

	// When parsing a block
	Block(const int& index, const time_t& timestamp, const std::string& previousHash, const std::string& hash, const std::vector<Transaction>& transactions);
	Block(const BlockHeader& header, const std::vector<Transaction>& transactions);
};

