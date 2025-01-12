#include "InventoryData.h"
#include <algorithm>

// Constructor
InventoryData::InventoryData()
{
}

// Get all Transaction IDs
std::vector<std::string> InventoryData::getTxIDs() const
{
    return _txIDs;
}

// Get all Block Hashes along with their Previous Block Hashes
std::vector<std::pair<std::string, std::string>> InventoryData::getBlocksHash() const
{
    return _blocksHash;
}

// Add a Transaction ID
void InventoryData::addTxID(const std::string& txID)
{
    // Check if txID already exists to prevent duplicates
    if (std::find(_txIDs.begin(), _txIDs.end(), txID) == _txIDs.end())
    {
        _txIDs.emplace_back(txID);
    }
}

// Remove a Transaction ID
void InventoryData::removeTxID(const std::string& txID)
{
    auto it = std::remove(_txIDs.begin(), _txIDs.end(), txID);
    if (it != _txIDs.end())
    {
        _txIDs.erase(it, _txIDs.end());
    }
}

// Set the entire list of Transaction IDs
void InventoryData::setTxIDs(const std::vector<std::string>& txIDs)
{
    _txIDs = std::move(txIDs);
}

// Add a Block Hash with its Previous Block Hash
void InventoryData::addBlock(const std::string& blockHash, const std::string& prevBlockHash)
{
    // Create a pair for the block
    std::pair<std::string, std::string> newBlock = { blockHash, prevBlockHash };

    // Check if the block already exists to prevent duplicates
    if (std::find(_blocksHash.begin(), _blocksHash.end(), newBlock) == _blocksHash.end())
    {
        _blocksHash.emplace_back(std::move(newBlock));
    }
}

// Remove a Block Hash with its Previous Block Hash
void InventoryData::removeBlock(const std::string& blockHash, const std::string& prevBlockHash)
{
    std::pair<std::string, std::string> targetBlock = { blockHash, prevBlockHash };
    auto it = std::remove(_blocksHash.begin(), _blocksHash.end(), targetBlock);
    if (it != _blocksHash.end())
    {
        _blocksHash.erase(it, _blocksHash.end());
    }
}

// Set the entire list of Block Hashes
void InventoryData::setBlocks(const std::vector<std::pair<std::string, std::string>>& blocksHash)
{
    _blocksHash = std::move(blocksHash);
}
