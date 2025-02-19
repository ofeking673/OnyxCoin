#include "pch.h"
#include "BlockHeader.h"

BlockHeader::BlockHeader(int index, time_t timestamp, const std::string& previousHash)
{
	_index = index;
	_timestamp = timestamp;
	_previousHash = previousHash;
}

BlockHeader::BlockHeader()
{
	_index = ERROR_BLOCK_HEADER;
	_timestamp = std::time(nullptr);
	_hash = std::to_string(ERROR_BLOCK_HEADER);
	_previousHash = std::to_string(ERROR_BLOCK_HEADER);
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

	BlockHeader header = BlockHeader(index, timestamp, previousHash);
	header.setHash(hash);

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
