#pragma once

#include "MessageP2P.h"
#include <string>

//#include "PeerManager.h"
#include "Networking/P2PNode.h"
#include "MessageManager.h"

#define FULL_NODE_MESSAGE_HANDLER 1

// Interface (pure virtual class) for handling each message type.
// Any class that wants to handle messages should inherit from this.
class IMessageHandler
{
public:
    IMessageHandler(P2PNode* node) : _node(node) {};
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

    virtual std::vector<MessageP2P> onHandshake(const MessageP2P& msg) = 0;

    virtual std::vector<MessageP2P> onPreprepare(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onPrepare(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onCommit(const MessageP2P& msg) = 0;
    //virtual std::vector<MessageP2P> onHashReady(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onNewView(const MessageP2P& msg) = 0;
    virtual std::vector<MessageP2P> onViewChange(const MessageP2P& msg) = 0;

    virtual std::vector<MessageP2P> onGetView(const MessageP2P& msg) = 0;
    // Utility for keeping the "alive" state updated for each node
    virtual std::string getPublicKey();

    void addPeer(PeerInfo info);

protected:
    P2PNode* _node;
};
