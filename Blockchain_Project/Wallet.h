#pragma once
#include "Blockchain.h"
#include "Point.h" //for cpp_int to handle private keys and message signature results in a point
#include "Encryptions/ECDSASigner.h"
#include "UTXO.h"
#include "AddressGenerator.h"

class Wallet 
{
public:
	// New wallet
	Wallet();

	// Login wallet
	Wallet(std::string seed, bool seedInitialize);
	Wallet(std::string privateKey);

	// TO-DO: Remove those functions!!!
	//		  Only for testing!
	//////////////////////////////////////////////////////////////////
	std::string getPrivateKey() { return _privateKey; }
	std::string getPublicKey() { return _publicKey; }
	std::string getAddress() { return _address; }
	std::string _seed;
	//////////////////////////////////////////////////////////////////
private:
	std::string _privateKey;
	std::string _publicKey;
	std::string _address;

	// Store which UTXOs belong to this address
	std::map<OutPoint, UTXOData> myUTXOs;

	std::string cppIntToHexString(cpp_int v);
	cpp_int hexStringToCppInt(std::string hex);

};