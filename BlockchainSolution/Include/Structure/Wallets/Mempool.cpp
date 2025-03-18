#include "pch.h"
#include "Mempool.h"

Mempool* Mempool::_instance = nullptr;

void Mempool::addTransaction(const Transaction& tx)
{
    // Reserve UTXOs referenced by this transaction
    for (const auto& input : tx.getInputs()) 
    {
        _reservedUTXOs[input.getPreviousOutPoint()] = tx.getTransactionID();
    }
    _pendingTransactions[tx.getTransactionID()] = tx;
}

void Mempool::removeTransaction(const std::string& txID)
{
    // Release reservations for UTXOs used in this transaction
    auto it = _pendingTransactions.find(txID);
    if (it != _pendingTransactions.end()) 
    {
        for (const auto& input : it->second.getInputs()) 
        {
            _reservedUTXOs.erase(input.getPreviousOutPoint());
        }
        _pendingTransactions.erase(it);
    }
}

bool Mempool::isUTXOReserved(const OutPoint& op) const
{
    return _reservedUTXOs.find(op) != _reservedUTXOs.end();
}

Transaction Mempool::getTransaction(const std::string& txID) const
{
    auto it = _pendingTransactions.find(txID);
    if (it != _pendingTransactions.end()) 
    {
        return it->second;
    }
    // Return error transaction if not found
    return Transaction();
}

std::unordered_map<std::string, Transaction> Mempool::getPendingTransactions() const
{
    return _pendingTransactions;
}

size_t Mempool::getPendingTransactionsAmount() const
{
    return _pendingTransactions.size();
}
