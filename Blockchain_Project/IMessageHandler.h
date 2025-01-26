#pragma once

#include "MessageP2P.h"
#include "string"

#include "PeerManager.h"

#define FULL_NODE_MESSAGE_HANDLER 1

// Interface (pure virtual class) for handling each message type.
// Any class that wants to handle messages should inherit from this.
class IMessageHandler
{
public:
    IMessageHandler(std::string keyPath, int port) : _peerManager(this, keyPath, port) {};
    virtual ~IMessageHandler() = default;

    virtual std::vector<MessageP2P> onError(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onPing(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onPong(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onGetPeers(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onPeerList(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onGetBlock(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onBlock(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onNewTransaction(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onGetTransaction(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onInventory(const MessageP2P& msg) = 0;
            
    virtual std::vector<MessageP2P> onGetHeaders(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onHeaders(const MessageP2P& msg) = 0;

    // Utility for keeping the "alive" state updated for each node
    virtual std::vector<SOCKET> getAllClients();
    virtual std::string getPublicKey();
    virtual std::string signMessage(std::string msg);

protected:
    PeerManager _peerManager;
    MessageManager _messageManager;
};
