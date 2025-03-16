#pragma once
#include <unordered_map>
#include <string>
#include "Transaction.h"
#include "OutPoint.h"

// This class represents a collection of pending transactions.
class Mempool {
public:
    Mempool() {};

    ~Mempool() = default;

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

private:
    // Pending transactions mapped by their transaction IDs.
    std::unordered_map<std::string, Transaction> _pendingTransactions;

    // Mapping of reserved UTXOs: OutPoint -> Transaction ID that reserved it.
    std::unordered_map<OutPoint, std::string, OutPointHash> _reservedUTXOs;
};
