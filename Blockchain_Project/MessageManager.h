#pragma once

#include <string>
#include <memory>
#include "MessageP2P.h"
#include "MessageTypes.h"
#include "Encryptions/ECDSASigner.h"
#include "Blockchain.h"
#include "InventoryData.h"

// The MessageManager is responsible for creating new P2P messages
// and handling incoming messages at a high level.
class MessageManager
{
public:
    MessageManager() = default;
    ~MessageManager() = default;

    // ------------------------------------------------------------------------
    // Create new messages (outgoing)
    // ------------------------------------------------------------------------

    /// Create a PING message
    MessageP2P createPingMessage(const std::string& privateKey);

    /// Create a PONG message
    MessageP2P createPongMessage(const std::string& privateKey, const std::string& time);

    /// Create a GET_PEERS message
    MessageP2P createGetPeersMessage(const std::string& privateKey);

    /// Create a PEER_LIST message
    MessageP2P createPeerListMessage(const std::string& privateKey);
        /*const std::vector<uint8_t>& serializedPeers);*/

    /// Create a GET_BLOCK message
    MessageP2P createGetBlockMessage(const std::string& privateKey,
        const std::string& blockHash,
        const std::string& prevBlockHash);

    /// Create a BLOCK message
    MessageP2P createBlockMessage(const std::string& privateKey,
        const Block& block);

    /// Create a NEW_TRANSACTION message
    MessageP2P createNewTransactionMessage(const std::string& privateKey,
        const Transaction& tx);

    /// Create a GET_TRANSACTION message
    MessageP2P createGetTransactionMessage(const std::string& privateKey,
        const std::string& txID);

    /// Create an INVENTORY message
    MessageP2P createInventoryMessage(const std::string& privateKey,
        const InventoryData& inventoryData);

    /// Create a GET_HEADERS message
    MessageP2P createGetHeadersMessage(const std::string& privateKey,
        /*const std::vector<uint8_t>& locatorData*/);

    /// Create a HEADERS message
    MessageP2P createHeadersMessage(const std::string& privateKey,
        /*const std::vector<uint8_t>& serializedHeaders*/);

    void signMessage(MessageP2P& messgae, const std::string& privateKey);

    // ------------------------------------------------------------------------
    // Receiving messages (incoming)
    // ------------------------------------------------------------------------

    /// A high-level method that receives a message and performs relevant actions:
    /// - Logs, updates peer manager, triggers a response, etc.
    /// You can later integrate this with a MessageDispatcher or IMessageHandler
    /// for more detailed logic.
    void receiveMessage(const MessageP2P& msg);

private:
    std::string getCurrentDateTime();
    // You might store references to other components here, e.g.:
    // PeerManager*    _peerManager;
    // NetworkManager* _networkManager;
    // etc.
};

