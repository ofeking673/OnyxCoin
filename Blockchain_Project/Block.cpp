#include "Block.h"

Block::Block(int index, const std::string& previousHash)
{
	_index = index;
	_previousHash = previousHash;
	_timestamp = std::time(nullptr);
	_hash = calculateHash();
}

Block::~Block()
{
	_transactions.clear();
}

void Block::addTransaction(const Transaction& transaction)
{
	_transactions.push_back(transaction);
}

std::string Block::calculateHash() const
{
	std::stringstream ss;

	ss << _index << _previousHash << _timestamp;
	for (const auto& tx : _transactions)
	{
		ss << tx.getTransactionID();
	}

	// TODO: change to use SHA-256 cryptographic function
	return ss.str();
}

void Block::displayBlock() const
{
	std::string time = HelperT::timeToStr(_timestamp);

	std::cout <<
		"Block #" << _index << std::endl <<
		"Timestamp: " << time << std::endl <<
		"Previous Hash: " << _previousHash << std::endl <<
		"Hash: " << _hash << std::endl <<
		"Transactions: " << std::endl;

	for (const auto& tx : _transactions)
	{
		tx.displayTransaction();
		std::cout << "-------------------------" << std::endl;
	}
}

std::string Block::getHash() const
{
	return _hash;
}

void Block::setHash(const std::string& hash)
{
	_hash = hash;
}

std::string Block::getPreviousHash() const
{
	return _previousHash;
}


