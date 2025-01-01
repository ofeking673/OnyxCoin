#pragma once
#include <string>
#include <map>
#include "OutPoint.h"
#include "UTXOData.h"
#include "Transaction.h"
#include "AddressGenerator.h"
#include "Encryptions/AES256CBC.h"
#include "Encryptions/Argon2.h"
#include "Encryptions/BIP39SeedMaker.h"

#define KEY_FILE_PATH "../WalletKey"
#define _CRT_SECURE_NO_WARNINGS

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
    //Seed initialize is needed in order to differentiate between normal wallet creation
    Wallet(const std::string& seed, bool seedInitialize);

    // 3) Creating wallet from existing private key
    Wallet(const std::string& filename); //Always input KEY_FILE_PATH unless mentioned otherwise

    ~Wallet() { saveWalletData(); };

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
    Transaction createTransaction(const std::string& toPublicKey, uint64_t amount);

    // Signs the input of a transaction at a given input index
    // The scriptSig can then be set inside the TxInput
    //void signInput(Transaction& tx, size_t inputIndex) const;

    // Update this wallet's UTXO set when a new transaction *to* this wallet arrives 
    // or when an existing UTXO is spent

    void updateUTXOsWithTransaction(const Transaction& tx);

    // A more general approach after each block is mined:
    //  - go through each transaction in that block
    //  - update your myUTXOs accordingly
    void updateUTXOsFromNewBlock(const std::vector<Transaction>& blockTransactions);

    // Utility: load wallet data (private key, UTXOs, etc.) from some data source
    void loadWalletData(const std::string& filename = KEY_FILE_PATH);

    // Utility: save wallet data (private key, UTXOs, etc.) for persistence
    void saveWalletData(const std::string& filename = KEY_FILE_PATH) const;

    // Utility: To avoid code repetitition, This function is used to initialize private key, seed, and public key.
    void calculateData();
private:

    // Internal function to pick the right set of UTXOs to spend for a given amount
    // If not enough funds, returns empty vector
    // Returns vector of outPoint, and the amount of them
    std::pair<std::vector<OutPoint>, size_t> selectUTXOs(uint64_t amount) const;

    // Removes a UTXO from myUTXOs if exists
    void removeUTXO(const OutPoint& outpoint);

private:
    std::string _privateKey;
    std::string _publicKey;
    std::string _address;

    // Store which UTXOs belong to this wallet's address
    // key   = OutPoint
    // value = UTXOData (contains value and scriptPubKey)
    std::map<OutPoint, UTXOData> myUTXOs;
};
