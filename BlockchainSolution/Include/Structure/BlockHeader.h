#pragma once
#include <string>
#include <ctime>
#include <vector>
#include "Networking/json.hpp"

#define ERROR_BLOCK_HEADER -1
#define MAX_HEADERS 200

class BlockHeader
{
public:
	BlockHeader(int index, time_t timestamp, const std::string& previousHash);
	BlockHeader(); // Error
	~BlockHeader() = default;
	int getIndex() const;
	time_t getTimeStamp() const;
	std::string getHash() const;
	std::string getPreviousHash() const;
	uint64_t getNonce() const;

	void setNonce(const uint64_t& nonce);
	void setHash(const std::string& hash);

	nlohmann::json toJson() const;
	static BlockHeader fromJson(nlohmann::json data);
	static nlohmann::json vectorToJson(const std::vector<BlockHeader>& blockHeaders);
	static std::vector<BlockHeader> jsonToVector(nlohmann::json data);


	// Copy Assignment Operator
	BlockHeader& operator=(const BlockHeader& other);

	// Compare this header with other block header. (Check if same block but the other can be mined)
	bool operator==(const BlockHeader& other) const;
private:
	int _index;
	time_t _timestamp;
	std::string _previousHash;
	std::string _hash;
	uint64_t _nonce;
};

