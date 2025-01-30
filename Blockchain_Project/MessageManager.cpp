#include "MessageManager.h"
#include "AddressGenerator.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

MessageP2P MessageManager::createPingMessage(const std::string& publicKey)
{
    // time = YYYY-MM-DD HH:MM:SS
    
    json j;
    // Time of message
    std::string time = getCurrentDateTime();
    j["time"] = time;

    MessageP2P message("", publicKey, MessageType::PING, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createPongMessage(const std::string& publicKey, const std::string& time)
{
    // payload = YYYY-MM-DD HH:MM:SS

    json j;
    // Time of PING message
    j["time"] = time;

    MessageP2P message("", publicKey, MessageType::PONG, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createGetPeersMessage(const std::string& publicKey)
{
    json j;

    MessageP2P message("", publicKey, MessageType::GET_PEERS, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createPeerListMessage(const std::string& publicKey, const json& serializedPeerList)
{
    MessageP2P message("", publicKey, MessageType::PEER_LIST, serializedPeerList.dump().length(), serializedPeerList);
    return message;
}

MessageP2P MessageManager::createGetBlockMessage(const std::string& publicKey, const std::string& blockHash, const std::string& prevBlockHash)
{
    // payload = blockHash|prevBlockHash

    json j;
    j["blockHash"] = blockHash;
    j["prevBlockHash"] = prevBlockHash;

    MessageP2P message("", publicKey, MessageType::GET_BLOCK, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createBlockMessage(const std::string& publicKey, const Block& block)
{
    json j = block.toJson();

    MessageP2P message("", publicKey, MessageType::BLOCK, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createNewTransactionMessage(const std::string& publicKey, const Transaction& tx)
{
    json j = tx.toJson();

    MessageP2P message("", publicKey, MessageType::NEW_TRANSACTION, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createGetTransactionMessage(const std::string& publicKey, const std::string& txID)
{
    json j;
    j["txID"] = txID;

    MessageP2P message("", publicKey, MessageType::GET_TRANSACTION, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createInventoryMessage(const std::string& publicKey, const InventoryData& inventoryData)
{
    json j = inventoryData.toJson();

    MessageP2P message("", publicKey, MessageType::INVENTORY, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createGetHeadersMessage(const std::string& publicKey, const std::vector<std::pair<std::string, std::string>>& blockHashes, const std::string& stopHash)
{
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

    MessageP2P message("", publicKey, MessageType::GET_HEADERS, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createHeadersMessage(const std::string& publicKey, const std::vector<BlockHeader>& blockHeaders)
{
    json j = BlockHeader::vectorToJson(blockHeaders);

    MessageP2P message("", publicKey, MessageType::HEADERS, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createHandshakeMessage(const std::string& publicKey, const PeerInfo& myPeerInfo)
{
    json j = myPeerInfo.toJson();

    MessageP2P message("", publicKey, MessageType::HANDSHAKE, j.dump().length(), j);
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
