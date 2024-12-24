#include "Wallet.h"

void Wallet::InitializeTransaction(std::string k2, double amt)
{
	Transaction trans(_k, k2, amt);
	Blockchain* chain = Blockchain::getInstance();
	chain->addTransaction(trans);
}

Wallet::Wallet()
{
	/*
	Generate random seed
	get private key
	calculate public key
	calculate address
	*/
	Wallet(seed, true);
}

Wallet::Wallet(std::string seed, bool seedInitialize)
{
	/*
	Get private key from seed
	calculate public key
	calculate address
	*/

	Wallet(privateKey);
}

Wallet::Wallet(std::string privateKey)
{
	/*
	Calculate public key
	calculate address
	*/

	cpp_int cppIntPublicKey = hexStringToCppInt(publicKey);
	address = AddressGenerator::generateAddress(cppIntPublicKey);
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

	boost::multiprecision::cpp_int num(hex, 16);
}
