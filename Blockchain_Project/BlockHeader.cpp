#include "BlockHeader.h"
#include "json.hpp"

using json = nlohmann::json;

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

std::string BlockHeader::toJson() const
{
	json j;
	j["index"] = _index;
	j["timestamp"] = static_cast<uint64_t>(_timestamp);
	j["previousHash"] = _previousHash;
	j["hash"] = _hash;
	return j.dump();
}

BlockHeader BlockHeader::fromJson(const std::string& data)
{
	// Parse the input JSON string
	json j = json::parse(data);

	// Extract the block properties
	int index = j["index"].get<int>();
	time_t timestamp = static_cast<time_t>(j["timestamp"].get<uint64_t>());
	std::string previousHash = j["previousHash"].get<std::string>();
	std::string hash = j["hash"].get<std::string>();

	BlockHeader header = BlockHeader(index, timestamp, previousHash);
	header.setHash(hash);

	return header;
}

std::string BlockHeader::vectorToJson(const std::vector<BlockHeader>& blockHeaders)
{
	json j;
	// Serialize all BlockHeaders using their toJson() functions
	json headers = json::array();
	for (const auto& header : blockHeaders)
	{
		// Convert the BlockHeader JSON string into a JSON object before pushing it
		headers.push_back(json::parse(header.toJson()));
	}
	j["headers"] = headers;

	return j.dump();
}
