#pragma once
#include "Blockchain.h"
#include "Point.h" //for cpp_int to handle private keys and message signature results in a point
#include "Encryptions/ECDSASigner.h"
#include "UTXO.h"
#include "AddressGenerator.h"

class Wallet 
{
public:
	//Wallet(std::string k) : _k(k) {}
	//void InitializeTransaction(std::string k2, double amt);

	// New wallet
	Wallet();

	// Login wallet
	Wallet(std::string seed, bool seedInitialize);
	Wallet(std::string privateKey);

private:
	/*std::string _k;*/
	std::string privateKey;
	std::string publicKey;
	std::string address;

	// Store which UTXOs belong to this address
	std::map<OutPoint, UTXOData> myUTXOs;

	std::string cppIntToHexString(cpp_int v);
	cpp_int hexStringToCppInt(std::string hex);

};