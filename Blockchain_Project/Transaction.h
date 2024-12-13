#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include "HelperT.h"

class Transaction
{
public:
	Transaction(const std::string& sender, const std::string receiver, double amount);
	~Transaction() {};

	void displayTransaction() const;

	std::string getTransactionID() const;

	std::string getSender() const { return _sender; }
	std::string getReceiver() const { return _receiver; }
	double getAmount() const { return _amount; }
private:
	std::string _sender;
	std::string _receiver;
	double _amount;
	time_t _timestamp;
	std::string _transactionID;


	std::string generateTransactionID();
};

