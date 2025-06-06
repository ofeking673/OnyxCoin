#pragma once
#include <unordered_map>
#include <string>
#include "Transaction.h"
#include "OutPoint.h"
#include <mutex>

// This class represents a collection of pending transactions.
class Mempool {
public:
    // Retrieve the singleton instance
    static Mempool* getInstance() {
        if (!_instance)
            _instance = new Mempool();
        return _instance;
    }

    // Delete copy constructor and assignment operator to enforce singleton
    Mempool(const Mempool&) = delete;
    Mempool& operator=(const Mempool&) = delete;


    // Add a new transaction to the mempool
    void addTransaction(const Transaction& tx);

    // Remove a transaction from the mempool
    void removeTransaction(const std::string& txID);

    // Check if a UTXO is reserved by a pending transaction
    bool isUTXOReserved(const OutPoint& op) const;

    // Retrieve a pending transaction by its ID
    Transaction getTransaction(const std::string& txID) const;

    // Get pending transactions
    std::unordered_map<std::string, Transaction> getPendingTransactions() const;

    // Get amount of pending transactions
    size_t getPendingTransactionsAmount() const;

    // When adding a new block to the chain, remove all pending transactions that are committed in the block
    void updateMempoolOnNewBlock(const Block& block);
private:
    // Private constructor to enforce singleton usage
    Mempool() = default;

    // The singleton instance
    static Mempool* _instance;

    // Allows the same thread to lock multiple times.
    // Declare as mutable so it can be locked in const functions.
    mutable std::recursive_mutex _mutex;

    // Pending transactions mapped by their transaction IDs.
    std::unordered_map<std::string, Transaction> _pendingTransactions;

    // Mapping of reserved UTXOs: OutPoint -> Transaction ID that reserved it.
    std::unordered_map<OutPoint, std::string, OutPointHash> _reservedUTXOs;
};
