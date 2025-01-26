#include "MessageManager.h"
#include "AddressGenerator.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "json.hpp"

using nlohmann::json;

MessageP2P MessageManager::createPingMessage(const std::string& publicKey)
{
    // time = YYYY-MM-DD HH:MM:SS
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::PING);
    message.setAuthor(address);
    
    json j;
    // Time of message
    std::string time = getCurrentDateTime();
    std::cout << time << std::endl;
    j["time"] = time;
    std::string payload = j.dump();
    std::cout << payload << std::endl;
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createPongMessage(const std::string& publicKey, const std::string& time)
{
    // payload = YYYY-MM-DD HH:MM:SS
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::PING);
    message.setAuthor(address);

    json j;
    // Time of PING message
    j["time"] = time;
    std::string payload = j.dump();
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}


MessageP2P MessageManager::createGetPeersMessage(const std::string& publicKey)
{
    MessageP2P message;
    message.setType(MessageType::GET_PEERS);
    message.setAuthor(publicKey);


    return message;
}

MessageP2P MessageManager::createGetBlockMessage(const std::string& publicKey, const std::string& blockHash, const std::string& prevBlockHash)
{
    // payload = blockHash|prevBlockHash
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::GET_BLOCK);
    message.setAuthor(address);

    json j;
    j["blockHash"] = blockHash;
    j["prevBlockHash"] = prevBlockHash;
    std::string payload = j.dump();

    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createBlockMessage(const std::string& publicKey, const Block& block)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::GET_BLOCK);
    message.setAuthor(address);

    std::string payload = block.toJson();
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createNewTransactionMessage(const std::string& publicKey, const Transaction& tx)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::NEW_TRANSACTION);
    message.setAuthor(address);

    std::string payload = tx.toJson();
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createGetTransactionMessage(const std::string& publicKey, const std::string& txID)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::GET_TRANSACTION);
    message.setAuthor(address);

    json j;
    j["txID"] = txID;
    std::string payload = j.dump();

    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createInventoryMessage(const std::string& publicKey, const InventoryData& inventoryData)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::INVENTORY);
    message.setAuthor(address);

    std::string payload = inventoryData.toJson();
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createGetHeadersMessage(const std::string& publicKey, const std::vector<std::pair<std::string, std::string>>& blockHashes, const std::string& stopHash)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::GET_HEADERS);
    message.setAuthor(address);

    json j;
    // Create a JSON array for blockHashes
    j["blockHashes"] = json::array();
    for (const auto& p : blockHashes) {
        j["blockHashes"].push_back({
            {"blockHash", p.first},
            {"prevBlockHash", p.second}
            });
    }
    // Assign the stopHash value
    j["stopHash"] = stopHash;


    std::string payload = j.dump();

    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}

MessageP2P MessageManager::createHeadersMessage(const std::string& publicKey, const std::vector<BlockHeader>& blockHeaders)
{
    MessageP2P message;

    std::string address = publicKey;

    message.setType(MessageType::HEADERS);
    message.setAuthor(address);


    std::string payload = BlockHeader::vectorToJson(blockHeaders);
    message.setLength(payload.length());
    message.setPayload(payload);

    return message;
}


std::string MessageManager::getCurrentDateTime()
{
    // Get the current time using the system clock.
    auto now = std::chrono::system_clock::now();

    // Convert to time_t which represents calendar time
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Convert to local time 
    std::tm tm_struct;
    #if defined(_MSC_VER)
        localtime_s(&tm_struct, &now_time);
    #else
        tm_struct = *std::localtime(&now_time);
    #endif

    // Use string stream and put_time to format the date and time
    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}
