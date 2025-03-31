#include "pch.h"
#include "Mempool.h"

Mempool* Mempool::_instance = nullptr;

void Mempool::addTransaction(const Transaction& tx)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Reserve UTXOs referenced by this transaction
    for (const auto& input : tx.getInputs()) 
    {
        _reservedUTXOs[input.getPreviousOutPoint()] = tx.getTransactionID();
    }
    _pendingTransactions[tx.getTransactionID()] = tx;
}

void Mempool::removeTransaction(const std::string& txID)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

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
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    return _reservedUTXOs.find(op) != _reservedUTXOs.end();
}

Transaction Mempool::getTransaction(const std::string& txID) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

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
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    return _pendingTransactions;
}

size_t Mempool::getPendingTransactionsAmount() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    return _pendingTransactions.size();
}

void Mempool::updateMempoolOnNewBlock(const Block& block)
{
    for (auto& tx : block._transactions)
    {
        removeTransaction(tx.getTransactionID());
    }
}
