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

	void displayBlock() const;

	std::string getHash() const;
	std::string getPreviousHash() const;
	void setHash(const std::string& hash);


private:
	int _index;
	time_t _timestamp;
	std::vector<Transaction> _transactions;
	std::string _previousHash;
	std::string _hash;
	
};

