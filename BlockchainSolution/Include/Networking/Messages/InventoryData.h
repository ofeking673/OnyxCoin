#pragma once
#include <vector>
#include <string>
#include "Networking/json.hpp"

class InventoryData
{
public:
	InventoryData();

	std::vector<std::string> getTxIDs() const;
	std::vector<std::pair<std::string, std::string>> getBlocksHash() const;
	
	void addTxID(const std::string& txID);
	void removeTxID(const std::string& txID);
	void setTxIDs(const std::vector<std::string>& txIDs);

	void addBlock(const std::string& blockHash, const std::string& prevBlockHash);
	void removeBlock(const std::string& blockHash, const std::string& prevBlockHash);
	void setBlocks(const std::vector<std::pair<std::string, std::string>>& blocksHash);

	std::string toMessageString() const;
	static InventoryData parseMessageString(const std::string& data);

	nlohmann::json toJson() const;
	static InventoryData fromJson(nlohmann::json data);
private:
	std::vector<std::string> _txIDs;
	std::vector<std::pair<std::string, std::string>> _blocksHash; // blockHash | prevBlockHash

	// When parsing InventoryData message
	InventoryData(const std::vector<std::string>& txIDs,
		const std::vector<std::pair<std::string, std::string>>& blocksHash);

};

