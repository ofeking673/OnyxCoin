#pragma once
#include "Blockchain.h"
#include "Point.h" //for cpp_int to handle private keys and message signature results in a point
#include "Encryptions/ECDSASigner.h"

class Wallet 
{
public:
	Wallet(std::string k) : _k(k) {}
	void InitializeTransaction(std::string k2, double amt);
private:
	std::string _k;
};