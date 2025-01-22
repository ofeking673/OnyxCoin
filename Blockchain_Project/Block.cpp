#include "Block.h"
#include "json.hpp"

using json = nlohmann::json;

Block::Block(int index, const std::string& previousHash) :
	_blockHeader(index, std::time(nullptr), previousHash)
{
	_blockHeader.setHash(calculateHash());
}

Block::Block() :
	_blockHeader(ERROR_BLOCK_HASH, std::time(nullptr), std::to_string(ERROR_BLOCK_HASH))
{
	_blockHeader.setHash(std::to_string(ERROR_BLOCK_HASH));
}

Block::Block(const int& index, const time_t& timestamp, const std::string& previousHash, const std::string& hash, const std::vector<Transaction>& transactions)
	: _blockHeader(index, timestamp, previousHash)
	, _transactions(transactions)
{
	_blockHeader.setHash(hash);
}

Block::~Block()
{
	_transactions.clear();
}

/// <summary>
/// Check if the block is indicating error
/// </summary>
/// <returns>True if error block, False if not error</returns>
bool Block::isErrorBlock() const
{
	if (_blockHeader.getIndex() == ERROR_BLOCK_HASH
		|| _blockHeader.getPreviousHash() == std::to_string(ERROR_BLOCK_HASH)
		|| _blockHeader.getHash() == std::to_string(ERROR_BLOCK_HASH))
	{
		return true;
	}
	return false;
}

void Block::addTransaction(const Transaction& transaction)
{
	_transactions.push_back(transaction);
}

std::string Block::calculateHash() const
{
	return SHA256::digest(getCurrentBlockInfo());
}

std::string Block::getCurrentBlockInfo() const
{
	std::stringstream ss;

	ss << _blockHeader.getIndex() << _blockHeader.getPreviousHash();
	for (const auto& tx : _transactions)
	{
		ss << tx.getTransactionID();
	}
	return ss.str();
}

void Block::displayBlock() const
{
	std::string time = HelperT::timeToStr(_blockHeader.getTimeStamp());

	std::cout <<
		"Block #" << _blockHeader.getIndex() << std::endl <<
		"Timestamp: " << time << std::endl <<
		"Previous Hash: " << _blockHeader.getPreviousHash() << std::endl <<
		"Hash: " << _blockHeader.getHash() << std::endl <<
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

	std::string block = _blockHeader.getIndex() + "|" + std::to_string(_blockHeader.getTimeStamp()) + "|" + _blockHeader.getPreviousHash() + "|" + _blockHeader.getHash() + "|" + transactions;
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

std::string Block::toJson() const
{
	// Create a JSON object for the block
	json j;
	j["index"] = _blockHeader.getIndex();
	j["timestamp"] = static_cast<uint64_t>(_blockHeader.getTimeStamp());
	j["previousHash"] = _blockHeader.getPreviousHash();
	j["hash"] = _blockHeader.getHash();

	// Serialize all transactions using their toJson() functions
	json transactionsJson = json::array();
	for (const auto& tx : _transactions)
	{
		// Convert the transaction JSON string into a JSON object before pushing it
		transactionsJson.push_back(json::parse(tx.toJson()));
	}
	j["transactions"] = transactionsJson;

	// Return the complete JSON string for the block
	return j.dump();
}

Block Block::fromJson(const std::string& data)
{
	// Parse the input JSON string
	json j = json::parse(data);

	// Extract the block properties
	int index = j["index"].get<int>();
	time_t timestamp = static_cast<time_t>(j["timestamp"].get<uint64_t>());
	std::string previousHash = j["previousHash"].get<std::string>();
	std::string hash = j["hash"].get<std::string>();

	// Reconstruct the list of transactions from the array in JSON.
	std::vector<Transaction> transactions;
	for (const auto& txJson : j["transactions"])
	{
		// Convert the JSON object back to a string for Transaction::fromJson
		Transaction tx = Transaction::fromJson(txJson.dump());
		transactions.push_back(tx);
	}

	// Use the private constructor to re-create the full block, including its timestamp and hash
	return Block(index, timestamp, previousHash, hash, transactions);
}

std::string Block::getHash() const
{
	return _blockHeader.getHash();
}

void Block::setHash(const std::string& hash)
{
	_blockHeader.getHash() = hash;
}

const BlockHeader Block::getBlockHeader() const
{
	return _blockHeader;
}

/// <summary>
/// Find transaction in transaction of the block, searching for transaction ID
/// </summary>
/// <param name="txID">Transaction ID searching for</param>
/// <returns>Transaction if found. Else returns error transaction</returns>
const Transaction Block::findTransaction(const std::string& txID) const
{
	auto it = std::find_if(_transactions.begin(), _transactions.end(), [txID](const Transaction& tx)
		{
			return tx.getTransactionID() == txID;
		});

	if (it != _transactions.end())
	{
		// Found the transaction
		return *it;
	}
	else
	{
		// Transaction ID not found
		return Transaction();
	}
}

std::string Block::getPreviousHash() const
{
	return _blockHeader.getPreviousHash();
}


