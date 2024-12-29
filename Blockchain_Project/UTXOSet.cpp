#include "UTXOSet.h"
#include <iostream>

UTXOSet* UTXOSet::_instance = nullptr;

void UTXOSet::addUTXO(const OutPoint& outpoint, const UTXOData& utxoData)
{
    // Insert or update the UTXO
    _utxoSet[outpoint] = utxoData;
}

void UTXOSet::removeUTXO(const OutPoint& outpoint)
{
    // Find and erase the UTXO if exists
    auto it = _utxoSet.find(outpoint);
    if (it != _utxoSet.end()) {
        _utxoSet.erase(it);
    }
}

bool UTXOSet::hasUTXO(const OutPoint& outpoint) const
{
    // Check if OutPoint is in the UTXO set
    return (_utxoSet.find(outpoint) != _utxoSet.end());
}

UTXOData UTXOSet::getUTXOData(const OutPoint& outpoint) const
{
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
    // Return a copy of the entire set
    return _utxoSet;
}

