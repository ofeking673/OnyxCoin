#pragma once

#include "IMessageHandler.h"
#include "Blockchain.h"
#include "UTXOSet.h"
#include "PeerManager.h"

// A concrete class that implements IMessageHandler,
// suitable for a full node in the blockchain network.
class FullNodeMessageHandler : public IMessageHandler
{
public:
    // Constructor and Destructor
    FullNodeMessageHandler(std::string keyPath, int port);
    virtual ~FullNodeMessageHandler();

    // Implement IMessageHandler Interface
    std::vector<MessageP2P> onError(const MessageP2P& msg) override { return{}; };

    std::vector<MessageP2P> onPing(const MessageP2P& msg) override;
    std::vector<MessageP2P> onPong(const MessageP2P& msg) override;
    
    std::vector<MessageP2P> onGetPeers(const MessageP2P& msg) override;
    std::vector<MessageP2P> onPeerList(const MessageP2P& msg) override;
    
    std::vector<MessageP2P> onGetBlock(const MessageP2P& msg) override;
    std::vector<MessageP2P> onBlock(const MessageP2P& msg) override;
    
    std::vector<MessageP2P> onNewTransaction(const MessageP2P& msg) override;
    std::vector<MessageP2P> onGetTransaction(const MessageP2P& msg) override;
    
    std::vector<MessageP2P> onInventory(const MessageP2P& msg) override;
    
    std::vector<MessageP2P> onGetHeaders(const MessageP2P& msg) override;
    std::vector<MessageP2P> onHeaders(const MessageP2P& msg) override;

    std::vector<MessageP2P> onHandshake(const MessageP2P& msg) override;
private:

    Blockchain* _blockchain;
    UTXOSet* _utxoSet;
    // Inherits from IMessageHandler:
    //     P2PNode _node;
};
