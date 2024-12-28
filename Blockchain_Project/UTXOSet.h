#pragma once
#include "UTXOData.h"
#include "OutPointHash.h"
#include <unordered_map>

class UTXOSet
{
public:
    // Add a new UTXO to the global UTXO set
    void addUTXO(const OutPoint& outpoint, const UTXOData& utxoData);

    // Remove a spent UTXO from the global set
    void removeUTXO(const OutPoint& outpoint);

    // Check if a given OutPoint is still unspent
    bool hasUTXO(const OutPoint& outpoint) const;

    // Retrieve UTXO data if it exists
    // If not wxists return UTXOData(0,0)
    UTXOData getUTXOData(const OutPoint& outpoint) const;

    // Return the entire map
    std::unordered_map<OutPoint, UTXOData, OutPointHash> getAllUTXOs() const;

public:
    // The global UTXO set: key = OutPoint, value = UTXOData
    // This represents all unspent outputs across the entire blockchain.
    std::unordered_map<OutPoint, UTXOData, OutPointHash> _utxoSet;
};
