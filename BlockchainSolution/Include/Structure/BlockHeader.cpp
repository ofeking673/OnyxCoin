#include "pch.h"
#include "BlockHeader.h"

BlockHeader::BlockHeader(int index, time_t timestamp, const std::string& previousHash)
{
	_index = index;
	_timestamp = timestamp;
	_previousHash = previousHash;
	_nonce = 0;
}

BlockHeader::BlockHeader()
{
	_index = ERROR_BLOCK_HEADER;
	_timestamp = std::time(nullptr);
	_hash = std::to_string(ERROR_BLOCK_HEADER);
	_previousHash = std::to_string(ERROR_BLOCK_HEADER);
	_nonce = 0;
}

int BlockHeader::getIndex() const
{
	return _index;
}

time_t BlockHeader::getTimeStamp() const
{
	return _timestamp;
}

std::string BlockHeader::getHash() const
{
	return _hash;
}

std::string BlockHeader::getPreviousHash() const
{
	return _previousHash;
}

uint64_t BlockHeader::getNonce() const
{
	return _nonce;
}

void BlockHeader::setNonce(const uint64_t& nonce)
{
	_nonce = nonce;
}

void BlockHeader::setHash(const std::string& hash)
{
	_hash = hash;
}

// Serialzie block header to Json
json BlockHeader::toJson() const
{
	json j;
	j["index"] = _index;
	j["timestamp"] = static_cast<uint64_t>(_timestamp);
	j["previousHash"] = _previousHash;
	j["hash"] = _hash;
	j["nonce"] = _nonce;
	return j;
}

// Deserialize block header from json
BlockHeader BlockHeader::fromJson(json data)
{
	// Parse the input JSON string
	json j = data;

	// Extract the block properties
	int index = j["index"].get<int>();
	time_t timestamp = static_cast<time_t>(j["timestamp"].get<uint64_t>());
	std::string previousHash = j["previousHash"].get<std::string>();
	std::string hash = j["hash"].get<std::string>();
	uint64_t nonce = j["nonce"].get<uint64_t>();

	BlockHeader header = BlockHeader(index, timestamp, previousHash);
	header.setHash(hash);
	header.setNonce(nonce);

	return header;
}

// Serialize vector of block headers to Json
json BlockHeader::vectorToJson(const std::vector<BlockHeader>& blockHeaders)
{
	json j;
	// Serialize all BlockHeaders using their toJson() functions
	json headers = json::array();
	for (const auto& header : blockHeaders)
	{
		// Convert the BlockHeader JSON string into a JSON object before pushing it
		headers.push_back(header.toJson());
	}
	j["headers"] = headers;

	return j;
}

// Deserialize json object to a vector of block headers
std::vector<BlockHeader> BlockHeader::jsonToVector(json data)
{
	std::vector<BlockHeader> blockHeaders;

	// Parse the input JSON string
	json j = data;

	// Iterate over each header in the JSON array
	for (const auto& headerJson : j["headers"]) 
	{
		// Deserialize to BlockHeader object
		BlockHeader header = BlockHeader::fromJson(headerJson);

		// Add to the vector
		blockHeaders.push_back(header);
	}

	return blockHeaders;
}


BlockHeader& BlockHeader::operator=(const BlockHeader& other)
{
	if (this == &other)
	{ // Self-assignment check
		return *this;
	}

	// Copy data
	_index = other._index;
	_timestamp = other._timestamp;
	_previousHash = other._previousHash;
	_hash = other._hash;
	_nonce = other._nonce;

	// Return the current object by reference
	return *this;
}

bool BlockHeader::operator==(const BlockHeader& other) const
{
	// Check without nonce and hash. Check if the other block is the mined block of the current block
	return
		_index == other._index &&
		_previousHash == other._previousHash;
}
