#pragma once

#include "MessageP2P.h"

// Interface (pure virtual class) for handling each message type.
// Any class that wants to handle messages should inherit from this.
class IMessageHandler
{
public:
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
};
