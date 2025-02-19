#pragma once

#include "Transaction.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <utility>
#include <map>

//struct UTXOData
//{
//	uint64_t value;
//	std::string scriptPubKey; // Locking script
//};


// The global UTXO set: key = OutPoint, value = UTXOData
// This represents all unspent outputs across the entire blockchain.
//using UTXOSet = std::unordered_map<OutPoint, UTXOData, OutPointHash>;

class UTXO
{
};

