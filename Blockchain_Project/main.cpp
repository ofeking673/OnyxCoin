#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "Encryptions/KeyGenerator.h"
#include "Encryptions/ECDSASigner.h"
#include "Encryptions/Base58.h"
#include "Encryptions/BIP39SeedMaker.h"
#include "Encryptions/Test.h"
#include "Encryptions/AES256CBC.h"
#include "Networking/ClientSocket.h"
#include "Networking/Socket.h"
#include "Networking/WSAInitializer.h"
#include "Wallet.h"

int main()
{
	//WSAInitializer wsa; //initialize WSA for socket usage

	Test test;
	test.testSha();
	test.testBip39();
	test.testEcc();
	test.testECDSA();










	//Transaction tx1("Alice", "Bob", 50);
	//Transaction tx2("Bob", "Charlie", 25);

	//myBlockchain.addTransaction(tx1);
	//myBlockchain.addTransaction(tx2);

	//myBlockchain.minePendingTransaction("Miner1");
	//
	//myBlockchain.displayBlockchain();

	//if (myBlockchain.isChainValid())
	//{
	//	std::cout << "Blockchain is valid" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Blockchain is invalid" << std::endl;
	//}
	
}