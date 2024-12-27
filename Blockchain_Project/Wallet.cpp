#include "Wallet.h"

Wallet::Wallet()
{
	/*
	Generate random private key
	get seed from private key
	calculate public key 
	calculate address
	*/
	KeyGenerator keyGenerator;
	BIP39SeedMaker bip39;

	std::cout << "Initializing new wallet..." << std::endl;

	// Generate private key
	cpp_int privateKey = keyGenerator.generatePrivate();
	_privateKey = cppIntToHexString(privateKey);

	// Transform private key to string
	std::string seed = bip39.transformToSeed(privateKey);
	std::cout << "REMEMBER YOUR SEED - to initialize when lost private key:" << std::endl
		<< seed << std::endl;

	//////////////////////////////////////////////////////////////////
	_seed = seed;
	//////////////////////////////////////////////////////////////////
	 
	
	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKey, keyGenerator.GPoint);
	_publicKey = pointPublicKey->ToString();

	// Calculate Address
	cpp_int publicKey = hexStringToCppInt(_publicKey);
	_address = AddressGenerator::generateAddress(publicKey);
}

Wallet::Wallet(std::string seed, bool seedInitialize)
{
	KeyGenerator keyGenerator;
	BIP39SeedMaker bip39;

	std::cout << "Regenerating wallet from seed..." << std::endl;

	// Get private key from seed
	_privateKey = bip39.reverseSeed(seed);
	cpp_int privateKey = hexStringToCppInt(_privateKey);

	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKey, keyGenerator.GPoint);
	_publicKey = pointPublicKey->ToString();

	// Calculate Address
	cpp_int publicKey = hexStringToCppInt(_publicKey);
	_address = AddressGenerator::generateAddress(publicKey);
}

Wallet::Wallet(std::string privateKey)
{
	/*
	Calculate public key
	calculate address
	*/
	KeyGenerator keyGenerator;
	BIP39SeedMaker bip39;

	std::cout << "Regenerating wallet from private key..." << std::endl;

	_privateKey = privateKey;

	cpp_int privateKeyCppInt = hexStringToCppInt(_privateKey);
	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKeyCppInt, keyGenerator.GPoint);
	_publicKey = pointPublicKey->ToString();

	// Calculate Address
	cpp_int publicKey = hexStringToCppInt(_publicKey);
	_address = AddressGenerator::generateAddress(publicKey);
}

std::string Wallet::cppIntToHexString(cpp_int v)
{
	std::string hexStr = v.str(16);
	return hexStr;
}

cpp_int Wallet::hexStringToCppInt(std::string hex)
{
	// Remove "0x" prefix if present
	if (hex.find("0x") == 0 || hex.find("0X") == 0) {
		hex = hex.substr(2);
	}

	cpp_int num = 5/*(hex, 16) */;
	return num;
}
