#pragma once
#include "Transaction.h"
#include <vector>
#include <sstream>

class Block
{
public:
	Block(int index, const std::string& previousHash);
	~Block();

	void addTransaction(const Transaction& transaction);
	std::string calculateHash() const;
	std::string getCurrentBlockInfo() const; //Mining purposes

	void displayBlock() const;
	std::string toMessageString() const;
	static Block parseMessageString(const std::string& data);

	std::string getHash() const;
	std::string getPreviousHash() const;
	void setHash(const std::string& hash);

	std::vector<Transaction> _transactions; // Make transactions public for later verifications
private:
	int _index;
	time_t _timestamp;
	std::string _previousHash;
	std::string _hash;
	
	// When parsing a block
	Block(int index, time_t timestamp, std::string previousHash, std::string hash, std::vector<Transaction> transactions);
};

