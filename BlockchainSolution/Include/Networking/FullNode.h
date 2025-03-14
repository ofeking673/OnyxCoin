#pragma once
#include "P2PNode.h"
#include "Structure/Wallets/Wallet.h"
#include "Structure/Wallets/Transaction.h"
#include "Messages/MessageManager.h"

class FullNode 
{
public:
    FullNode(const std::string& walletFilePath, const std::string& listenAddress, uint16_t port);

    ~FullNode();

    // CLI command to print wallet balance
    void showBalance() const;

    // CLI command to shou user info
    void showUserInfo() const;

    // CLI command to print pending transactions
    void showPendingTransactions() const;

    // CLI command to create and broadcast a transaction
    void createAndBroadcastTransaction(const std::string& recipientPublicKey, uint64_t amount);

    // Update wallet UTXO set when new block or tx received (to be hooked in dispatcher)
    void processIncomingTransaction(const Transaction& tx);


    // Accessor for external checks (if needed).
    bool isRunning() const;

    // The command-line interface loop running in its own thread.
    void runCLI();

private:
    //Wallet _wallet;
    P2PNode _p2pNode;
    std::atomic<bool> _running;
    std::thread _cliThread;
};


