#pragma once
#include <string>
#include <ctime>
#include <vector>

#define ERROR_BLOCK_HEADER -1

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

	void setHash(const std::string& hash);

	std::string toJson() const;
	static BlockHeader fromJson(const std::string& data);
	static std::string vectorToJson(const std::vector<BlockHeader>& blockHeaders);

private:
	int _index;
	time_t _timestamp;
	std::string _previousHash;
	std::string _hash;
};

