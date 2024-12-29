#pragma once
#include "Blockchain.h"
#include "Point.h" //for cpp_int to handle private keys and message signature results in a point
#include "Encryptions/ECDSASigner.h"
#include "UTXO.h"
#include "AddressGenerator.h"
#include "UTXOData.h"

class Wallet 
{
public:
	//// New wallet
	//Wallet();

	//// Login wallet
	//Wallet(std::string seed, bool seedInitialize);
	//Wallet(std::string privateKey);

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


#pragma once
#include <string>
#include <map>
#include "OutPoint.h"
#include "UTXOData.h"
#include "Transaction.h"

class Wallet
{
public:
    // TO-DO: Remove this  !!!
    //		  Only for testing!
    //////////////////////////////////////////////////////////////////
    std::string _seed;
    //////////////////////////////////////////////////////////////////
    
    // Constructors:
    // 1) New wallet
    Wallet();

    // 2) Creating wallet from seed
    Wallet(const std::string& seed, bool seedInitialize);

    // 3) Creating wallet from existing private key
    Wallet(const std::string& privateKey);

    ~Wallet() = default;

    // Returns the address associated with this wallet
    std::string getAddress() const;

    // Returns the current public key
    std::string getPublicKey() const;

    // Returns the current private key (in practice, keep this encrypted/safe)
    std::string getPrivateKey() const;

    // Returns the total balance (sum of all UTXOs belonging to this wallet)
    uint64_t getBalance() const;

    // Creates a new transaction to the specified recipient address with the given amount
    //  - selects enough UTXOs from myUTXOs
    //  - creates the inputs
    //  - creates an appropriate output (and a change output if needed)
    //  - signs the inputs
    //  - returns a fully-formed Transaction object
    Transaction createTransaction(const std::string& toAddress, uint64_t amount);

    // Signs the input of a transaction at a given input index
    // The scriptSig can then be set inside the TxInput
    void signInput(Transaction& tx, size_t inputIndex) const;

    // Update this wallet's UTXO set when a new transaction *to* this wallet arrives 
    // or when an existing UTXO is spent
    //  - If a TxOutput's scriptPubKey matches my address, add to myUTXOs
    //  - If a TxInput references one of my outpoints, remove that from myUTXOs
    void updateUTXOsWithTransaction(const Transaction& tx);

    // A more general approach after each block is mined:
    //  - go through each transaction in that block
    //  - update your myUTXOs accordingly
    void updateUTXOsFromNewBlock(const std::vector<Transaction>& blockTransactions);

    // Utility: load wallet data (private key, UTXOs, etc.) from some data source
    void loadWalletData(const std::string& filename);

    // Utility: save wallet data (private key, UTXOs, etc.) for persistence
    void saveWalletData(const std::string& filename) const;

private:

    // Internal function to pick the right set of UTXOs to spend for a given amount
    // If not enough funds, returns empty vector
    std::vector<OutPoint> selectUTXOs(uint64_t amount) const;

private:
    std::string _privateKey;
    std::string _publicKey;
    std::string _address;

    // Store which UTXOs belong to this wallet's address
    // key   = OutPoint
    // value = UTXOData (contains value and scriptPubKey)
    std::map<OutPoint, UTXOData> myUTXOs;
};
