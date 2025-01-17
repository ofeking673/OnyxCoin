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

    virtual void onError(const MessageP2P& msg) = 0;

    virtual void onPing(const MessageP2P& msg) = 0;
    virtual void onPong(const MessageP2P& msg) = 0;

    virtual void onGetPeers(const MessageP2P& msg) = 0;
    virtual void onPeerList(const MessageP2P& msg) = 0;

    virtual void onGetBlock(const MessageP2P& msg) = 0;
    virtual void onBlock(const MessageP2P& msg) = 0;

    virtual void onNewTransaction(const MessageP2P& msg) = 0;
    virtual void onGetTransaction(const MessageP2P& msg) = 0;

    virtual void onInventory(const MessageP2P& msg) = 0;

    virtual void onGetHeaders(const MessageP2P& msg) = 0;
    virtual void onHeaders(const MessageP2P& msg) = 0;

    // Utility for keeping the "alive" state updated for each node
    virtual std::vector<SOCKET> getAllClients();
    virtual std::string getPublicKey();
    virtual std::string signMessage(std::string msg);

protected:
    PeerManager _peerManager;
    MessageManager _messageManager;
};
