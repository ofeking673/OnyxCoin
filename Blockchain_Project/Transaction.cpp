#include "Transaction.h"

Transaction::Transaction(const std::string& sender, const std::string receiver, double amount)
{
	_sender = sender;
	_receiver = receiver;
	_amount = amount;
	_timestamp = std::time(nullptr);
	_transactionID = generateTransactionID();
}

void Transaction::displayTransaction() const
{
	std::cout << "Transaction ID: " << _transactionID << std::endl;
	std::cout << _sender << " -> " << _receiver << " : " << _amount << std::endl;
	std::cout << "Timestamp: " << std::ctime(&_timestamp) << std::endl;
}

// TO-DO: Change to use hash functions
std::string Transaction::generateTransactionID()
{
	return _sender + _receiver + std::to_string(_amount) + std::to_string(_timestamp);
}


