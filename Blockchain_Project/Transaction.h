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

#include "json.hpp"

#define TRANSACTION_ID_LENGTH 16 // 16 bytes
#define TAX_RATE 0.5

// Transaction types (first 1 byte in scriptPubKey)
#define REGULARE_TRANSACTION_TYPE 10 
#define REWARD_TRANSACTION_TYPE 20
#define CHANGE_TRANSACTION_TYPE 30

#define ERROR_TRANSACTION_ID -1
class Transaction
{
public:
	// Construct a transaction from given inputs and outputs
	Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs);
	Transaction(const Transaction& other);
	Transaction();
	~Transaction() = default;

	bool isErrorTransaction() const;

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
	void setTimestamp(time_t timestamp);

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

	// Checks that each input's scriptSig is correct with respect to the output's scriptPubKey
	bool verifyTransactionSignature(/*const std::string& scriptPubKey*/);


	std::string toString() const;
	nlohmann::json toJson() const;
	static Transaction fromJson(nlohmann::json jsonTx);

	std::string toMessageString() const;
	static Transaction parseMessageString(const std::string& data);

	static std::string hashPublicKey(const std::string& hexPubKey);

	// scriptPubKey = <type (1 byte)><public key hash (20 bytes)>
	static std::string extractPublicKeyHash(const std::string& scriptPubKey);
	static std::string extractTransactionType(const std::string& scriptPubKey);


	// Function to use after creating new transaction
	// Checks if the Transaction creation failed
	bool isTransactionCreationSucceeded();

	// Returns the transaction info, with empty scriptSig in inputs. hashed with sha256. To sign with ECDSA.
	std::string transactionMessageToSign();

	// Helper for generating transaction ID
	std::string generateTransactionID();
	// Helper for refreshing transaction ID
	void refreshTransactionID();


	bool operator==(const Transaction& other) const;

private:
	std::vector<TxInput>  _inputs;
	std::vector<TxOutput> _outputs;
	time_t                _timestamp;
	std::string           _transactionID;

	// When parsing a transaction
	Transaction(const std::string& transactionID, const time_t& timestamp, const std::vector<TxInput>& inputs, const std::vector<TxOutput>& outputs);
};
