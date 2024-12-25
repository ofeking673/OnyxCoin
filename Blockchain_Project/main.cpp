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
	WSAInitializer wsa; //initialize WSA for socket usage

	Test test;
	//test.testSha();
	//test.testBip39();
	//test.testEcc();
	//test.testECDSA();
	//test.testBlake2b();
	//test.testRIPEMD160();
	test.testWalletCreation();






	Blockchain* myBlockchain = Blockchain::getInstance();
	
	Transaction tx1("abc", "System", 50);

	myBlockchain->addTransaction(tx1);
	
	myBlockchain->displayBlockchain();

	if (myBlockchain->isChainValid())
	{
		std::cout << "Blockchain is valid" << std::endl;
	}
	else
	{
		std::cout << "Blockchain is invalid" << std::endl;
	}
	
}