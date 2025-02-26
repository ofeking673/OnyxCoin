#include "pch.h"
#include "MessageManager.h"


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

MessageP2P MessageManager::createDiscoveryRequestMessage(const std::string& publicKey, const PeerInfo& myPeerInfo)
{
    json j;
    j[DISCOVERY_PAYLOAD_LISTEN_IP] = myPeerInfo.ip;
    j[DISCOVERY_PAYLOAD_LISTEN_PORT] = myPeerInfo.port;
    MessageP2P message("", publicKey, MessageType::DISCOVERY_REQUEST, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createLeaderMessage(const std::string& publickey, const Block& block, MessageType type, const uint32_t currentView)
{
    json j;
    j["CURRENT_VIEW"] = currentView;
    j["BLOCK"] = block.toJson();
    j["SEQUENCE"] = block.getBlockHeader().getIndex();
    MessageP2P message("", publickey, type, j.dump().length(), j);
    return message;
}

MessageP2P MessageManager::createViewChange(const std::string& publickey, const uint32_t currentView, const int lastStableSeq, const std::string& checkpointDigest)
{
    json j;
    j["NEW_VIEW"] = currentView + 1;
    j["LAST_SEQ"] = lastStableSeq;
    j["CHECKPOINT"] = checkpointDigest;
    return MessageP2P("", publickey, MessageType::VIEW_CHANGE, j.dump().length(), j);
}

MessageP2P MessageManager::createNewView(const std::string& publickey, const uint32_t newView, const std::vector<MessageP2P>& viewChangeMessages)
{
    json j;

    json arr = json::array();
    for (auto& msg : viewChangeMessages) 
    {
        arr.push_back(msg.toJson());
    }

    j["NEW_VIEW"] = newView;
    j["VC_MESSAGES"] = arr;

    return MessageP2P("", publickey, MessageType::NEW_VIEW, j.dump().length(), j);
}

MessageP2P MessageManager::createGetView(const std::string& publickey)
{
    return MessageP2P("", publickey, MessageType::GET_VIEW, 0, json());
}

//MessageP2P MessageManager::createHashReadyMessage(const std::string& publickey, const PeerInfo& myPeerInfo, std::string hash, std::string blockID)
//{
//    json j;
//    j["HASH"] = hash;
//    j["BLOCK_ID"] = blockID;
//    return MessageP2P("", publickey, MessageType::HASH_READY, j.dump().length(), j);
//}


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
