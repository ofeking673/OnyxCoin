#pragma once

#include "IMessageHandler.h"
#include "PeerManager.h"
#include "Blockchain.h"
#include "UTXOSet.h"

// A concrete class that implements IMessageHandler,
// suitable for a full node in the blockchain network.
class FullNodeMessageHandler : public IMessageHandler
{
public:
    // Constructor and Destructor
    FullNodeMessageHandler(std::string keyPath);
    virtual ~FullNodeMessageHandler();

    // Implement IMessageHandler Interface
    void onPing(const MessageP2P& msg) override;
    void onPong(const MessageP2P& msg) override;

    void onGetPeers(const MessageP2P& msg) override;
    void onPeerList(const MessageP2P& msg) override;

    void onGetBlock(const MessageP2P& msg) override;
    void onBlock(const MessageP2P& msg) override;

    void onNewTransaction(const MessageP2P& msg) override;
    void onGetTransaction(const MessageP2P& msg) override;

    void onInventory(const MessageP2P& msg) override;

    void onGetHeaders(const MessageP2P& msg) override;
    void onHeaders(const MessageP2P& msg) override;

private:
    /*
    PeerManager (tracks connected peers, their addresses, last-seen timestamps, etc.)
    Blockchain (manages the chain of blocks, including validation and storage)
    TransactionPool or Mempool (temporary store for unconfirmed transactions)
    Network or P2PManager (responsible for actually sending messages across the network)
    */

    Blockchain* _blockchain;
    UTXOSet* _utxoSet;
    PeerManager peerManager;
    // NetworkManager
};
