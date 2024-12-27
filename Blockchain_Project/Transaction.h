#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"
#include "Encryptions/Blake2b.h"

#include "OutPoint.h"
#include "OutPointHash.h"
#include "TxInput.h"
#include "TxOutput.h"

#define TRANSACTION_ID_LENGTH 16 // 16 bytes

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

	// TO-DO: Remove!!! Only to fix errors for now
	//////////////////////////////////////////////
	Transaction(std::string, std::string, int) {};
	//////////////////////////////////////////////

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

