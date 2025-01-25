#pragma once
#include "Transaction.h"
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
	std::string toJson() const;
	static Block fromJson(const std::string& data);

	std::string getHash() const;
	std::string getPreviousHash() const;
	void setHash(const std::string& hash);

	const BlockHeader getBlockHeader() const;

	const Transaction findTransaction(const std::string& txID) const;

	std::vector<Transaction> _transactions; // Make transactions public for later verifications
private:
	//int _index;
	//time_t _timestamp;
	//std::string _previousHash;
	//std::string _hash;
	BlockHeader _blockHeader;

	// When parsing a block
	Block(const int& index, const time_t& timestamp, const std::string& previousHash, const std::string& hash, const std::vector<Transaction>& transactions);
};

