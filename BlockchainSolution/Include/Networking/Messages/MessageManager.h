#pragma once

#include <string>
#include <memory>
#include "MessageP2P.h"
#include "Encryptions/ECDSASigner.h"
#include "Structure/Blockchain.h"
#include "InventoryData.h"
#include "Networking/PeerInfo.h"

// The MessageManager is responsible for creating new P2P messages
// and handling incoming messages at a high level.
// Because we cannot pass the private key to any function, due to this manager
// being inside of the P2P node, and we would need to propogate this data,
// Each node should be capable of signing a message on its own, only using this class to create 
// the bare-bones structure of a message
class MessageManager
{
public:
    // ------------------------------------------------------------------------
    // Create new messages (outgoing)
    // ------------------------------------------------------------------------

    /// Create a PING message
    static MessageP2P createPingMessage(const std::string& publicKey);

    /// Create a PONG message
    static MessageP2P createPongMessage(const std::string& publicKey, const std::string& time);

    /// Create a GET_PEERS message
    static MessageP2P createGetPeersMessage(const std::string& publicKey);

    /// Create a PEER_LIST message
    static MessageP2P createPeerListMessage(const std::string& publicKey, const json& serializedPeerList);

    /// Create a GET_BLOCK message
    static MessageP2P createGetBlockMessage(const std::string& publicKey,
        const std::string& blockHash,
        const std::string& prevBlockHash);

    /// Create a BLOCK message
    static MessageP2P createBlockMessage(const std::string& publicKey,
        const Block& block);

    /// Create a NEW_TRANSACTION message
    static MessageP2P createNewTransactionMessage(const std::string& publicKey,
        const Transaction& tx);

    /// Create a GET_TRANSACTION message
    static MessageP2P createGetTransactionMessage(const std::string& publicKey,
        const std::string& txID);

    /// Create an INVENTORY message
    static MessageP2P createInventoryMessage(const std::string& publicKey,
        const InventoryData& inventoryData);

    /// Create a GET_HEADERS message
    static MessageP2P createGetHeadersMessage(const std::string& publicKey,
        const std::vector<std::pair<std::string, std::string>>& blockHashes,
        const std::string& stopHash);

    /// Create a HEADERS message
    static MessageP2P createHeadersMessage(const std::string& publicKey,
        const std::vector<BlockHeader>& blockHeaders);

    // Creates HANDSHAKE message
    static MessageP2P createHandshakeMessage(const std::string& publicKey, const PeerInfo& myPeerInfo);

    static MessageP2P createDiscoveryRequestMessage(const std::string& publicKey, const PeerInfo& myPeerInfo);

    static MessageP2P createLeaderMessage(const std::string& publickey, const Block& block, MessageType type, const uint32_t currentView);
    static MessageP2P createViewChange(const std::string& publickey, const uint32_t currentView, const int lastStableSeq, const std::string& checkpointDigest);
    static MessageP2P createNewView(const std::string& publickey, const uint32_t newView, const std::vector<MessageP2P>& viewChangeMessages);
    
    static MessageP2P createGetView(const std::string& publickey);
private:
    // Private constructor to prevent instantiation; all methods are static.
    MessageManager() = default;

    static std::string getCurrentDateTime();
};

