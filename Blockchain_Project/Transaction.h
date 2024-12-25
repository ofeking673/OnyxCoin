#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"
#include "Encryptions/Blake2b.h"

#define TRANSACTION_ID_LENGTH 16 // 16 bytes

using TxID = std::string;

// Represents an out point - reference to a specific transaction output
struct OutPoint
{
	TxID txid;		// ID (hash) of the transaction
	uint32_t index;	// Index of the output in that transaction

	bool operator==(const OutPoint& other) const
	{
		return (txid == other.txid && index == other.index);
	}
};

// A custom hash for OutPoint so it can be used in unordered map
struct OutPointHash
{
	std::size_t operator()(const OutPoint& op) const
	{
		std::hash<std::string> str_hash;
		return (str_hash(op.txid) ^ (std::hash<uint32_t>()(op.index) << 1));
	}
};

// A transaction output (what become UTXO if unspent)
struct TxOutput
{
	uint64_t value;
	std::string scriptPubKey;
};


struct TxInput
{
	OutPoint previousOutPoint; // Which UTXO this input is spending
	std::string scriptSig;	   

};

// Print OutPoint
std::ostream& operator<<(std::ostream& os, OutPoint op)
{
	os << "OutPoint(txid: " << op.txid << ", index: " << op.index << ")";
	return os;
}

// Print TxInput
std::ostream& operator<<(std::ostream& os, TxInput input) {
	os << "TxInput(";
	os << "previousOutpoint: " << input.previousOutPoint << ", ";
	os << "scriptSig: " << input.scriptSig;
	os << ")";
	return os;
}

// Print TxOutput
std::ostream& operator<<(std::ostream& os, const TxOutput& output) {
	os << "TxOutput(";
	os << "value: " << output.value << ", ";
	os << "scriptPubKey: " << output.scriptPubKey;
	os << ")";
	return os;
}

class Transaction
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="inputs">Vector of input transactions to use</param>
	/// <param name="outputs">Vector of output transactions to create</param>
	Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs);
	~Transaction() {};

	/// <summary>
	/// Display transaction
	/// </summary>
	void displayTransaction() const;

	/// <summary>
	/// Get transaction ID
	/// </summary>
	/// <returns>Transaction ID</returns>
	std::string getTransactionID() const;

private:
	std::vector<TxInput> _inputs;
	std::vector<TxOutput> _outputs;
	time_t _timestamp;
	std::string _transactionID;

	/// <summary>
	/// Generates the transaction ID
	/// </summary>
	/// <returns>Transaction ID</returns>
	std::string generateTransactionID();
};

