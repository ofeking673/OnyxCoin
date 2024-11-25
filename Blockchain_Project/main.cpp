//#include "Blockchain.h"
//
//int main()
//{
//	//Blockchain myBlockchain;
//
//	//Transaction tx1("Alice", "Bob", 50);
//	//Transaction tx2("Bob", "Charlie", 25);
//
//	//myBlockchain.addTransaction(tx1);
//	//myBlockchain.addTransaction(tx2);
//
//	//myBlockchain.minePendingTransaction("Miner1");
//	//
//	//myBlockchain.displayBlockchain();
//
//	//if (myBlockchain.isChainValid())
//	//{
//	//	std::cout << "Blockchain is valid" << std::endl;
//	//}
//	//else
//	//{
//	//	std::cout << "Blockchain is invalid" << std::endl;
//	//}
//}

#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "KeyGenerator.h"
#include "ECDSASigner.h"
#include "Base58.h"
#include "BIP39SeedMaker.h"
#include "Test.h"

int main()
{
	KeyGenerator* keyGen = new KeyGenerator();
	SHA256* sha = new SHA256();
	ECDSASigner* sign = new ECDSASigner();
	BIP39SeedMaker* bip = new BIP39SeedMaker();


	//cpp_int privateKey(keyGen->generatePrivate()); //Generate a new private key

	//std::cout << "New private key: " << privateKey << std::endl;


	//Point* publicKey = keyGen->ECMul(privateKey, keyGen->GPoint);
	//std::cout << "Public key for that private key:\n  X: " << publicKey->_x << "\n  Y: " << publicKey->_y << std::endl << std::endl;
	////   std::cout << "Extracting signature for message 'abc'...\n";
	////   std::string str = "abc";
	////   Point* rs = sign->signMessage(privateKey, str);
	////   std::cout << "Signature values: R = " << rs->_x << " S = " << rs->_y <<std::endl << std::endl;
	////   std::cout << "Is the message signed correctly? " << ((sign->verifySignature(rs, sha->digest(str), publicKey)) ? "Yes" : "No") << std::endl;

	//cpp_int pub = KeyGenerator::cat(publicKey->_x, publicKey->_y);
	//std::cout << pub << std::endl;
	//Base58 b;
	//std::cout << b.base58(pub);

	/*BIP39SeedMaker seeder;
	std::string seed = seeder.transformToSeed(cpp_int("0x26219df363d0b3d313f77f5f0abe4b82"));
	std::cout << seed << std::endl << std::endl;
	std::cout << seeder.reverseSeed(seed);*/

	Test t;
	t.testECDSA();
	return 0;
}