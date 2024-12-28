#include "Wallet.h"
#include <algorithm>

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

Wallet::Wallet(const std::string& seed, bool seedInitialize)
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

Wallet::Wallet(const std::string& privateKey)
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



// Getters
std::string Wallet::getAddress() const
{
	return _address;
}

std::string Wallet::getPublicKey() const
{
	return _publicKey;
}

std::string Wallet::getPrivateKey() const
{
	return _privateKey;
}

uint64_t Wallet::getBalance() const
{
	size_t balance = 0;
	for (const auto& utxo : myUTXOs)
	{
		balance += utxo.second.getValue();
	}
	return balance;
}






/*
 * Internal function to pick the right set of UTXOs to spend for a given amount.
 *   1) Sort myUTXOs by value (ascending).
 *   2) Iterate from smallest to largest, accumulating until we've reached or exceeded 'amount'.
 */
std::vector<OutPoint> Wallet::selectUTXOs(uint64_t amount) const
{
	// Create a vector to hold all (OutPoint, value) pairs
	std::vector<std::pair<OutPoint, uint64_t>> utxos;
	utxos.reserve(myUTXOs.size());

	// Populate the vector
	for (const auto& kv : myUTXOs)
	{
		const OutPoint& outpoint = kv.first;
		const UTXOData& data = kv.second;
		utxos.emplace_back(outpoint, data.getValue());
	}

	// Sort by ascending value
	std::sort(utxos.begin(), utxos.end(),
		[](const auto& a, const auto& b) {
			return a.second < b.second;
		});

	uint64_t accumulated = 0;
	std::vector<OutPoint> selected;

	// Pick UTXOs from smallest to largest
	for (const auto& [outpoint, value] : utxos)
	{
		selected.push_back(outpoint);
		accumulated += value;

		if (accumulated >= amount)
		{
			break;
		}
	}

	// If we couldn't accumulate enough, throw or handle the error
	if (accumulated < amount)
	{
		std::cout << "Insufficient funds in the wallet to select the requested amount." << std::endl;
		return std::vector<OutPoint>(); // Returns empty vector
	}

	return selected;
}




std::string Wallet::cppIntToHexString(cpp_int v)
{
	std::string hexStr = v.str(16);
	return hexStr;
}

cpp_int Wallet::hexStringToCppInt(const std::string& hex)
{
	std::string hexStr;
	// Remove "0x" prefix if present
	if (hex.find("0x") == 0 || hex.find("0X") == 0) {
		hexStr = hex.substr(2);
	}

	cpp_int num = (hexStr, 16);
	return num;
}











