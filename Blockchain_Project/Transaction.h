#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"
#include "Encryptions/Blake2b.h"
#include "Encryptions/ECDSASigner.h"

#include "OutPoint.h"
#include "OutPointHash.h"
#include "TxInput.h"
#include "TxOutput.h"

#define TRANSACTION_ID_LENGTH 16 // 16 bytes

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

	void signTransaction(const std::string& privateKey);
	bool verifyTransactionSignature(const std::string& publicKey);


	std::string toString() const;
	std::string toJson() const;

private:
	// Helper for generating transaction ID
	std::string generateTransactionID();


	std::vector<TxInput>  _inputs;
	std::vector<TxOutput> _outputs;
	time_t                _timestamp;
	std::string           _transactionID;
};
