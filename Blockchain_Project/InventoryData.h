#pragma once
#include <vector>
#include <string>

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

	std::string toJson() const;
	static InventoryData fromJson(const std::string& data);
private:
	std::vector<std::string> _txIDs;
	std::vector<std::pair<std::string, std::string>> _blocksHash; // blockHash | prevBlockHash

	// When parsing InventoryData message
	InventoryData(const std::vector<std::string>& txIDs,
		const std::vector<std::pair<std::string, std::string>>& blocksHash);

};

