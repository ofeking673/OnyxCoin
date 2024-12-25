#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"

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

class Transaction
{
public:
	Transaction(const std::string& sender, const std::string receiver, double amount);
	~Transaction() {};

	void displayTransaction() const;

	std::string getTransactionID() const;

	//std::string getSender() const { return _sender; }
	//std::string getReceiver() const { return _receiver; }
	//double getAmount() const { return _amount; }
private:
	//std::string _sender;
	//std::string _receiver;
	//double _amount;
	//time_t _timestamp;
	//std::string _transactionID;

	std::vector<TxInput> inputs;
	std::vector<TxOutput> outputs;
	time_t _timestamp;
	std::string _transactionID;

	std::string generateTransactionID();
};

