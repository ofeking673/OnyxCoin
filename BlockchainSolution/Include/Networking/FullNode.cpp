#include "pch.h"
#include "FullNode.h"

FullNode::FullNode(const std::string& walletFilePath, const std::string& listenAddress, uint16_t port)
    : /*_wallet(walletFilePath),*/
    _p2pNode(false, walletFilePath),
    _running(true)
{
    if (!_p2pNode.start(listenAddress, port)) 
    {
        throw std::runtime_error("Failed to start P2P node");
    }
    // Start the CLI in its own thread.
    _cliThread = std::thread(&FullNode::runCLI, this);
}

FullNode::~FullNode()
{
    // Signal the CLI thread to stop and wait for it.
    _running = false;
    if (_cliThread.joinable()) {
        _cliThread.join();
    }
    _p2pNode.stop();
}

void FullNode::showBalance() const
{
    std::cout << "Balance: " << _p2pNode.m_myWallet.getBalance() << std::endl;
}

void FullNode::showUserInfo() const
{
    PeerInfo myInfo = _p2pNode.getMyInfo();
    std::cout << "[Info] User info: "
        << "\n  IP: " << myInfo.ip
        << "\n  Port: " << myInfo.port
        << "\n  Public Key: " << myInfo.publicKey
        << "\n  Node ID: " << myInfo.nodeId << std::endl << std::endl;
}

void FullNode::createAndBroadcastTransaction(const std::string& recipientPublicKey, uint64_t amount)
{
    Transaction tx = _p2pNode.m_myWallet.createTransaction(recipientPublicKey, amount);
    std::cout << "[Info] Transaction created" << std::endl;
    if (tx.getInputs().empty() && tx.getOutputs().empty()) 
    {
        std::cerr << "[Error] Transaction creation failed (insufficient funds?)" << std::endl;
        return;
    }
    // Create a P2P message wrapping the transaction
    MessageP2P newTxMessage = MessageManager::createNewTransactionMessage(_p2pNode.m_myWallet.getPublicKey(), tx);
    std::cout << __FUNCTION__": [Info] Broadcasting message!\n";
    _p2pNode.broadcastMessage(newTxMessage);
    std::cout << "[Info] Transaction broadcasted." << std::endl;
}


void FullNode::processIncomingTransaction(const Transaction& tx)
{
    _p2pNode.m_myWallet.updateUTXOsWithTransaction(tx);
}

bool FullNode::isRunning() const
{
    return _running;
}

void FullNode::runCLI()
{
    std::cout << "Welcome to the blockchain CLI." << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  balance                   - Show wallet balance" << std::endl;
    std::cout << "  tx <recipient> <amount>   - Create and broadcast a transaction" << std::endl;
    //std::cout << "  utxo                      - Update all transaction UTXO's" << std::endl;
    std::cout << "  view                      - Get current view" << std::endl;
    std::cout << "  leader                    - Get current leader's public key" << std::endl;
    std::cout << "  blockchain                - Print the blockchain to screen" << std::endl;
    std::cout << "  info                      - Show the user info" << std::endl;
    std::cout << "  exit                      - Quit" << std::endl;

    std::string line;
    while (_running) 
    {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) 
        {
            // If input fails (e.g., EOF), exit the loop.
            break;
        }
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        if (command == "balance") 
        {
            // Show the balance
            showBalance();
        }
        else if (command == "tx") 
        {
            std::string recipient;
            uint64_t amount;
            iss >> recipient >> amount;
            if (recipient.empty() || amount == 0) 
            {
                std::cerr << "[Error] Invalid parameters for transaction." << std::endl;
            }
            else 
            {
                // Create the transaction and broadcast it
                createAndBroadcastTransaction(recipient, amount);
            }
        }
        else if (command == "leader")
        {
            PeerInfo leaderInfo = _p2pNode.getPeerInfoByID(_p2pNode.getLeaderIndex());
            std::cout << "[Info] Leader's public key is: " << leaderInfo.publicKey << std::endl;
        }
        else if (command == "info")
        {
            showUserInfo();
        }
        else if (command == "exit") 
        {
            _running = false;
        }
        //else if (command == "utxo") {
        //    _p2pNode.m_myWallet.updateUTXOsFromNewBlock(_p2pNode.getLastBlock()._transactions);
        //}
        else if (command == "view") {

        }
        else if (command == "blockchain") 
        {
            _p2pNode.getBlockchain()->displayBlockchain();
        }
        else 
        {
            std::cerr << "[Error] Unknown command." << std::endl;
        }
    }
}
