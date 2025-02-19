#include "InventoryData.h"
#include "HelperT.h"
#include <algorithm>

using json = nlohmann::json;

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

std::string InventoryData::toMessageString() const
{
    // inventory = txIDs|blockHashes
    // txIDs = txid&...
    // blockHashes = hash^prevHash&...
    std::string txIDs = "";
    for (const auto& txid : _txIDs)
    {
        txIDs += txid + "&";
    }
    txIDs.pop_back();

    std::string blockHashes = "";
    for (const auto& block : _blocksHash)
    {
        blockHashes += block.first + "^" + block.second + "&";
    }
    blockHashes.pop_back();

    std::string inventory = txIDs + "|" + blockHashes;
    return inventory;
}

InventoryData InventoryData::parseMessageString(const std::string& data)
{
    // The overall format is: txIDs + "|" + blockHashes
    // Split using the pipe '|' delimiter.
    std::vector<std::string> parts = HelperT::split(data, '|');
    if (parts.size() != 2) 
    {
        // Invalid inventory format: expecting 'txIDs|blockHashes'
        return InventoryData();
    }

    // Parse the txIDs
    std::string txIDsStr = parts[0];
    std::vector<std::string> txIDs;
    if (!txIDsStr.empty()) 
    {
        txIDs = HelperT::split(txIDsStr, '&');
    }

    // Parse the block hashes
    std::string blocksHashStr = parts[1];
    std::vector<std::pair<std::string, std::string>> blocksHash;
    if (!blocksHashStr.empty()) 
    {
        // Each block hash entry is separated by '&'
        std::vector<std::string> blockTokens = HelperT::split(blocksHashStr, '&');
        for (const auto& token : blockTokens) 
        {
            // Each token has the format: blockHash^prevBlockHash
            std::vector<std::string> hashParts = HelperT::split(token, '^');
            if (hashParts.size() != 2) 
            {
                // Invalid block hash format in inventory data
                return InventoryData();
            }
            blocksHash.push_back({ hashParts[0], hashParts[1] });
        }
    }

    return InventoryData(txIDs, blocksHash);
}

json InventoryData::toJson() const
{
    json j;

    // Directly serializing vector<string>
    j["txIDs"] = _txIDs;

    // Serialize each block as an object with "blockHash" and "prevBlockHash"
    json blocksJson = json::array();
    for (const auto& blockPair : _blocksHash)
    {
        json blockObj;
        blockObj["blockHash"] = blockPair.first;
        blockObj["prevBlockHash"] = blockPair.second;
        blocksJson.push_back(blockObj);
    }
    j["blocksHash"] = blocksJson;

    // Return the complete JSON string
    return j;
}

InventoryData InventoryData::fromJson(json data)
{
    // Parse the incoming JSON string into a JSON object
    json j = data;

    // Extract the txIDs array
    std::vector<std::string> txIDs = j["txIDs"].get<std::vector<std::string>>();

    // Extract the blocks hash array
    std::vector<std::pair<std::string, std::string>> blocksHash;
    for (const auto& item : j["blocksHash"])
    {
        std::string blockHash = item["blockHash"].get<std::string>();
        std::string prevBlockHash = item["prevBlockHash"].get<std::string>();
        blocksHash.push_back({ blockHash, prevBlockHash });
    }

    // Use the parsing constructor to construct the InventoryData object
    return InventoryData(txIDs, blocksHash);
}

InventoryData::InventoryData(const std::vector<std::string>& txIDs, const std::vector<std::pair<std::string, std::string>>& blocksHash)
    : _txIDs(txIDs)
    , _blocksHash(blocksHash)
{
}
