#pragma once

#include <string>
#include <memory>
#include "MessageP2P.h"
#include "MessageTypes.h"

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
    MessageP2P createPingMessage(const std::string& signature);

    /// Create a PONG message
    MessageP2P createPongMessage(const std::string& signature);

    /// Create a GET_PEERS message
    MessageP2P createGetPeersMessage(const std::string& signature);

    /// Create a PEER_LIST message
    MessageP2P createPeerListMessage(const std::string& signature,
        const std::vector<uint8_t>& serializedPeers);

    /// Create a GET_BLOCK message
    MessageP2P createGetBlockMessage(const std::string& signature,
        const std::vector<uint8_t>& blockIdentifier);

    /// Create a BLOCK message
    MessageP2P createBlockMessage(const std::string& signature,
        const std::vector<uint8_t>& serializedBlock);

    /// Create a NEW_TRANSACTION message
    MessageP2P createNewTransactionMessage(const std::string& signature,
        const std::vector<uint8_t>& serializedTx);

    /// Create a GET_TRANSACTION message
    MessageP2P createGetTransactionMessage(const std::string& signature,
        const std::vector<uint8_t>& txID);

    /// Create an INVENTORY message
    MessageP2P createInventoryMessage(const std::string& signature,
        const std::vector<uint8_t>& inventoryData);

    /// Create a GET_HEADERS message
    MessageP2P createGetHeadersMessage(const std::string& signature,
        const std::vector<uint8_t>& locatorData);

    /// Create a HEADERS message
    MessageP2P createHeadersMessage(const std::string& signature,
        const std::vector<uint8_t>& serializedHeaders);

    // ------------------------------------------------------------------------
    // Receiving messages (incoming)
    // ------------------------------------------------------------------------

    /// A high-level method that receives a message and performs relevant actions:
    /// - Logs, updates peer manager, triggers a response, etc.
    /// You can later integrate this with a MessageDispatcher or IMessageHandler
    /// for more detailed logic.
    void receiveMessage(const MessageP2P& msg);

private:
    // You might store references to other components here, e.g.:
    // PeerManager*    _peerManager;
    // NetworkManager* _networkManager;
    // etc.
};

