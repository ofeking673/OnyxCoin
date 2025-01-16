#include "Block.h"

Block::Block(int index, const std::string& previousHash)
{
	_index = index;
	_previousHash = previousHash;
	_timestamp = std::time(nullptr);
	_hash = calculateHash();
}

Block::Block(const int& index, const time_t& timestamp, const std::string& previousHash, const std::string& hash, const std::vector<Transaction>& transactions)

	: _index(index)
	, _timestamp(timestamp)
	, _previousHash(previousHash)
	, _hash(hash)
	, _transactions(transactions)
{
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
	// TODO: change to use SHA-256 cryptographic function
	return SHA256::digest(getCurrentBlockInfo());
}

std::string Block::getCurrentBlockInfo() const
{
	std::stringstream ss;

	ss << _index << _previousHash;
	for (const auto& tx : _transactions)
	{
		ss << tx.getTransactionID();
	}
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

std::string Block::toMessageString() const
{
	// block = _index|time|_previousHash|_hash|transactions;
	// transaction = txid$timestamp$inputs$outputs&...
	// inputs = i*previousPointTxID*previousPointIndex*scriptSig^...
	// outputs = i*value*scriptPubKey^...
	std::string transactions = "";
	for (const auto& tx : _transactions)
	{
		transactions += tx.toMessageString() + "&";
	}
	transactions.pop_back();

	std::string block = _index + "|" + std::to_string(_timestamp) + "|" + _previousHash + "|" + _hash + "|" + transactions;
	return block;
}

Block Block::parseMessageString(const std::string& data)
{
	// The overall delimiter between the main fields is '|'.
	// Expected format: index|time|previousHash|hash|transactions
	std::vector<std::string> parts = HelperT::split(data, '|');
	if (parts.size() != 5) {
		throw std::runtime_error("Invalid block format: Incorrect number of main fields");
	}

	// Parse index and timestamp
	int index = std::stoi(parts[0]);
	time_t timestamp = static_cast<time_t>(std::stoll(parts[1]));
	std::string previousHash = parts[2];
	std::string hash = parts[3];
	std::string transactionsStr = parts[4];

	std::vector<Transaction> transactions;

	// Parse transactions (each transaction is separated by '&')
	if (!transactionsStr.empty()) {
		std::vector<std::string> transactionsTokens = HelperT::split(transactionsStr, '&');
		for (const auto& txToken : transactionsTokens) {
			Transaction tx = Transaction::parseMessageString(txToken);
			transactions.push_back(tx);
		}
	}

	// Construct and return the Block object
	return Block(index, timestamp, previousHash, hash, transactions);
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


