#pragma once

#include <string>
#include <ctime>

class Transaction
{
public:
	Transaction(const std::string& sender, const std::string receiver, double amount);
	~Transaction() {};

	std::string generateTransactionID();
	void displayTransaction() const;

private:
	std::string sender;
	std::string receiver;
	double amount;
	time_t timestamp;
	std::string transactionID;
};

