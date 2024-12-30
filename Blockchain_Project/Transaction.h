#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"
#include "Encryptions/Blake2b.h"
#include "Encryptions/ECDSASigner.h"
#include "Encryptions/RIPEMD_160.h"

#include "OutPoint.h"
#include "OutPointHash.h"
#include "TxInput.h"
#include "TxOutput.h"

#define TRANSACTION_ID_LENGTH 16 // 16 bytes
#define TAX_RATE 0.01
class Transaction
{
public:
	// Construct a transaction from given inputs and outputs
	Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs);

	~Transaction() = default;

	// Return all inputs
	std::vector<TxInput> getInputs() const;

	// Return all outputs
	std::vector<TxOutput> getOutputs() const;

	// Get transaction ID (the unique identifier)
	std::string getTransactionID() const;

	// Print or log transaction details
	void displayTransaction() const;

	// Return the timestamp
	time_t getTimestamp() const;

	// Add a new input
	void addInput(const TxInput& input);

	// Add a new output
	void addOutput(const TxOutput& output);
	OutPoint generateOutpoint(const TxOutput& output); 

	//Calculate transaction tax (ooo scaryyy)
	uint64_t calculateTax() const;

	// Check if TxOutput is in transaction output list, If it exists, return it's index
	int getOutputIndex(const TxOutput& output);

	void signTransaction(const std::string& privateKey);
	bool verifyTransactionSignature();


	std::string toString() const;
	std::string toJson() const;
	static Transaction fromJson(const std::string& jsonStr);
	static std::string hashPublicKey(const std::string& hexPubKey);

	// scriptPubKey = <type (1 byte)><public key (64 bytes)>
	static std::string extractTransactionType(const std::string& scriptPubKey);
	static std::string extractPubKeyHash(const std::string& scriptPubKey);

private:
	// Helper for generating transaction ID
	std::string generateTransactionID();


	std::vector<TxInput>  _inputs;
	std::vector<TxOutput> _outputs;
	time_t                _timestamp;
	std::string           _transactionID;
};
