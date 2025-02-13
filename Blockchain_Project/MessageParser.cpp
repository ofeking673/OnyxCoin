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

const void MessageParser::parseViewChangeMessage(const MessageP2P& msg, uint32_t& newView, int& lastStableSeq, std::string& checkpointDigest)
{
    // Get the paramaters of view_change message (new view, last stable sequence, and the hash of the last checkpoint block)
    newView = msg.getPayload()["NEW_VIEW"].get<uint32_t>();
    lastStableSeq = msg.getPayload()["LAST_SEQ"].get<int>();
    checkpointDigest = msg.getPayload()["CHECKPOINT"].get<std::string>();
}

const void MessageParser::parseNewViewMessage(const MessageP2P& msg, uint32_t& newView, std::vector<MessageP2P>& viewChangeMessages)
{
    newView = msg.getPayload()["NEW_VIEW"].get<uint32_t>();

    // Check if the view change messages vector exists and is an array
    if (msg.getPayload().contains("VC_MESSAGES") && msg.getPayload()["VC_MESSAGES"].is_array()) {
        // Iterate through each element in the array
        for (auto& element : msg.getPayload()["VC_MESSAGES"]) {
            // Convert the JSON element to MyClass using fromJson
            MessageP2P vcmsg = MessageP2P::fromJson(element);
            // Append to the vector
            viewChangeMessages.push_back(vcmsg);
        }
    }
}
