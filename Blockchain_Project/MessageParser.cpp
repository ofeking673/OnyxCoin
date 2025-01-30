#include "MessageParser.h"
#include "MessageP2P.h"
#include <cstdint>
#include "json.hpp"

using nlohmann::json;

MessageP2P MessageParser::parse(const std::string& buffer)
{
    MessageP2P msg = MessageP2P::fromJson(json::parse(buffer));
    return msg;
}

const std::string MessageParser::parseGetTransactionMessage(const MessageP2P& msg)
{
    // Parse the JSON string back into a JSON object
    json j = msg.getPayload();

    // Retrieve the value for "txID"
    std::string parsedTxID = j["txID"].get<std::string>();

    return parsedTxID;
}

const std::pair<std::string, std::string> MessageParser::parseGetBlockMessage(const MessageP2P& msg)
{
    // Parse the JSON string back into a JSON object
    json j = msg.getPayload();

    // Retrieve the values for block hash and previous block hash
    std::string blockHash = j["blockHash"].get<std::string>();
    std::string prevBlockHash = j["prevBlockHash"].get<std::string>();


    std::pair<std::string, std::string> blkHash;
    blkHash.first = blockHash;
    blkHash.second = prevBlockHash;

    return blkHash;
}

const void MessageParser::parseGetHeadersMessage(const MessageP2P& msg, std::vector<std::pair<std::string, std::string>>& blockHashes, std::string& stopHash)
{
    // Parse the JSON string into a json object.
    json j = msg.getPayload();


    // Clear blockHashes and stopHash in case they have previous data.
    blockHashes.clear();
    stopHash.clear();

    // Check if the expected keys exist.
    if (!j.contains("blockHashes") || !j.contains("stopHash")) 
    {
        return;
    }

    // Parse stopHash.
    stopHash = j["stopHash"].get<std::string>();

    // Parse the blockHashes array.
    // Each element is expected to be an object containing "blockHash" and "prevBlockHash"
    for (const auto& element : j["blockHashes"]) 
    {
        std::string blockHash = element.at("blockHash").get<std::string>();
        std::string prevBlockHash = element.at("prevBlockHash").get<std::string>();
        blockHashes.emplace_back(blockHash, prevBlockHash);
    }
}
