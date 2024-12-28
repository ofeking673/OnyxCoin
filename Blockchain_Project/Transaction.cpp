#include "Transaction.h"

Transaction::Transaction(std::vector<TxInput> inputs, std::vector<TxOutput> outputs)
	: _inputs(std::move(inputs))
	, _outputs(std::move(outputs))
	, _timestamp(std::time(nullptr))
{
	_transactionID = generateTransactionID();
}

std::vector<TxInput> Transaction::getInputs() const
{
	return _inputs;
}

std::vector<TxOutput> Transaction::getOutputs() const
{
	return _outputs;
}

std::string Transaction::getTransactionID() const
{
	return _transactionID;
}

time_t Transaction::getTimestamp() const
{
	return _timestamp;
}

void Transaction::addInput(const TxInput& input)
{
	_inputs.push_back(input);
}

void Transaction::addOutput(const TxOutput& output)
{
	_outputs.push_back(output);
}

void Transaction::displayTransaction() const
{
	std::string time = HelperT::timeToStr(_timestamp);
	std::cout << "Transaction ID: " << _transactionID << std::endl 
		<< "Time stamp: " << time << std::endl << std::endl;

	std::cout << "Inputs: " << std::endl;
	for (auto input : _inputs)
	{
		std::cout << input << std::endl;
	}
	std::cout << "Outputs: " << std::endl;
	for (auto output : _outputs)
	{
		std::cout << output << std::endl;
	}

	std::cout << std::endl;
}


std::string Transaction::generateTransactionID()
{
	std::stringstream ss;

	// Include all transasction data in ss
	for (auto input : _inputs)
	{
		ss << input;
	}
	for (auto output : _outputs)
	{
		ss << output;
	}
	ss << _timestamp;



	// Hash the data to create a 16 bytes transaction ID

	// Convert ss into uint8_t*
	std::string strTx = ss.str();
	uint8_t* buffer = new uint8_t[strTx.size()];
	std::memcpy(buffer, strTx.data(), strTx.size());

	// Create output hash buffer
	uint8_t* hashedID = new uint8_t[TRANSACTION_ID_LENGTH];

	// Hash the transaction data
	Blake2b::hash(buffer, strTx.size(), hashedID, TRANSACTION_ID_LENGTH);

	//  Convert transaction ID to hex string
	std::string transactionID = Blake2b::bytesToHex(hashedID, TRANSACTION_ID_LENGTH);

	// Clear memory used
	delete[] buffer;
	delete[] hashedID;

	return transactionID;
}


