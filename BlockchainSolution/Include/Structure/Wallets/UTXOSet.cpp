#include "pch.h"
#include "UTXOSet.h"

UTXOSet* UTXOSet::_instance = nullptr;

void UTXOSet::addUTXO(const OutPoint& outpoint, const UTXOData& utxoData)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Insert or update the UTXO
    _utxoSet[outpoint] = utxoData;
}

void UTXOSet::removeUTXO(const OutPoint& outpoint)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Find and erase the UTXO if exists
    auto it = _utxoSet.find(outpoint);
    if (it != _utxoSet.end()) {
        _utxoSet.erase(it);
    }
}

bool UTXOSet::hasUTXO(const OutPoint& outpoint) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Check if OutPoint is in the UTXO set
    return (_utxoSet.find(outpoint) != _utxoSet.end());
}

UTXOData UTXOSet::getUTXOData(const OutPoint& outpoint) const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Retrieve the UTXOData
    auto it = _utxoSet.find(outpoint);
    if (it == _utxoSet.end())
    {
        std::cout << "Requested OutPoint does not exist in UTXO set: "
            << outpoint.getTxID() << " index: "
            << std::to_string(outpoint.getIndex()) << std::endl;
        return UTXOData(0, 0); // UTXOData(0, 0) when not found
    }
    return it->second;
}

std::unordered_map<OutPoint, UTXOData, OutPointHash> UTXOSet::getAllUTXOs() const
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Return a copy of the entire set
    return _utxoSet;
}

