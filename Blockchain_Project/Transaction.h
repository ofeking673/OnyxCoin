#pragma once

#include <string>
#include <ctime>
#include <iostream>

class Transaction
{
public:
	Transaction(const std::string& sender, const std::string receiver, double amount);
	~Transaction() {};

	void displayTransaction() const;

private:
	std::string _sender;
	std::string _receiver;
	double _amount;
	time_t _timestamp;
	std::string _transactionID;


	std::string generateTransactionID();
};

