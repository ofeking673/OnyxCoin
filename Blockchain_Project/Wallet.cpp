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
	_privateKey = ECDSASigner::cppIntToHexString(privateKey);

	// Transform private key to string
	std::string seed = bip39.transformToSeed(privateKey); //cpp_int("0x" +hex string);
	std::cout << "REMEMBER YOUR SEED - to initialize when lost private key:" << std::endl
		<< seed << std::endl;

	//////////////////////////////////////////////////////////////////
	_seed = seed;
	//////////////////////////////////////////////////////////////////
	 
	
	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKey, keyGenerator.GPoint);
	std::cout <<std::hex << pointPublicKey->_x << std::endl << std::hex << pointPublicKey->_y << std::endl; 
	_publicKey = pointPublicKey->ToStringNoPad();

	// Calculate Address
	_address = AddressGenerator::generateAddress(_publicKey);
}

Wallet::Wallet(const std::string& seed, bool seedInitialize)
{
	KeyGenerator keyGenerator;
	BIP39SeedMaker bip39;

	std::cout << "Regenerating wallet from seed..." << std::endl;

	// Get private key from seed
	_privateKey = bip39.reverseSeed(seed);
	cpp_int privateKey = ECDSASigner::hexStringToCppInt(_privateKey);

	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKey, keyGenerator.GPoint);
	_publicKey = pointPublicKey->ToStringNoPad();

	// Calculate Address
	_address = AddressGenerator::generateAddress(_publicKey);
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

	cpp_int privateKeyCppInt = ECDSASigner::hexStringToCppInt(_privateKey);
	// Calculate public key
	Point* pointPublicKey = keyGenerator.ECMul(privateKeyCppInt, keyGenerator.GPoint);
	_publicKey = pointPublicKey->ToStringNoPad();

	// Calculate Address
 	_address = AddressGenerator::generateAddress(_publicKey);
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

Transaction Wallet::createTransaction(const std::string& toPublicKey, uint64_t amount)
{
	std::pair<std::vector<OutPoint>, size_t> selectedUTXOs = selectUTXOs(amount);
	std::vector<OutPoint> outPoints = selectedUTXOs.first;
	size_t accumulated = selectedUTXOs.second;

	if (accumulated == 0) // Not enough coins in wallet
	{
		// Return Empty transaction
		return Transaction(std::vector<TxInput>(), std::vector<TxOutput>());
	}

	// Set inputs for transaction
	std::vector<TxInput> inputs;
	for (size_t i = 0; i < outPoints.size(); i++)
	{
		TxInput input(outPoints[i], ""); // Currently empty scriptSig. will be added using signTransaction
		inputs.push_back(input);
	}

	// Set outputs for transaction
	std::vector<TxOutput> outputs;
	//		Main output
	//			scriptPubKey => type(1 byte)|pubKeyHash(20 bytes)
	std::string scriptPubKey = std::to_string(REGULARE_TRANSACTION_TYPE) + Transaction::hashPublicKey(toPublicKey);
	TxOutput output(amount, scriptPubKey);
	outputs.push_back(output);

	// If we need a change, create output for it
	if (accumulated > amount)
	{
		// Create change transaction
		std::string scriptPubKey = std::to_string(REGULARE_TRANSACTION_TYPE) + Transaction::hashPublicKey(_publicKey /*We are the recievers*/);
		TxOutput change(accumulated - amount, scriptPubKey);
		outputs.push_back(change);
	}

	// Create the transaction with the correct inputs and outputs
	Transaction tx(inputs, outputs);

	// Sign the transaction
	tx.signTransaction(_privateKey);

	return tx;
}






/*
 * Internal function to pick the right set of UTXOs to spend for a given amount.
 *   1) Sort myUTXOs by value (ascending).
 *   2) Iterate from smallest to largest, accumulating until we've reached or exceeded 'amount'.
 */
std::pair<std::vector<OutPoint>, size_t> Wallet::selectUTXOs(uint64_t amount) const
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

	std::pair<std::vector<OutPoint>, size_t> selectedUtxos;
	// If we couldn't accumulate enough, throw or handle the error
	if (accumulated < amount)
	{
		std::cout << "Insufficient funds in the wallet to select the requested amount." << std::endl;
		selectedUtxos.second = 0;
		return selectedUtxos; // Returns empty pair
	}

	selectedUtxos.first = selected;			// Selected UTXOs
	selectedUtxos.second = accumulated;		// Accumalated sum of the OutPoints
	return selectedUtxos;
}