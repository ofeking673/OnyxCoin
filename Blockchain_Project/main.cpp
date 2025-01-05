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
#include "Server.h"
#include "Networking/WSAInitializer.h"
#include "Wallet.h"


void main2();
Server* serv;
int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage

	//Test test;
	//test.testSha();
	//test.testBip39();
	//test.testEcc();
	//test.testECDSA();
	//test.testBlake2b();
	//test.testRIPEMD160();
	//test.testWalletCreation();
	Blockchain* chain = Blockchain::getInstance();

	std::thread thread(main2);
	thread.detach();
	std::cout << "Server creation started!\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	Miner miner("../miner", 8026);
	Client client(8026, "../client1");
	Client client2(8026, "../client2");

	chain->testTransaction(client.wallet->getPublicKey(), 10);
	miner.mine();

	std::this_thread::sleep_for(std::chrono::seconds(5));

	std::cout << "Miner: " << miner.displayBalance() << std::endl <<
		"Client1: " << client.displayBalance() << std::endl <<
		"Client2: " << client2.displayBalance() << std::endl;

	chain->displayBlockchain();

	client.initializeTransaction(client2.wallet->getPublicKey(), 5);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	miner.mine();
	std::this_thread::sleep_for(std::chrono::seconds(5));

	std::cout << "Miner: " << miner.displayBalance() << std::endl <<
		"Client1: " << client.displayBalance() << std::endl <<
		"Client2: " << client2.displayBalance() << std::endl;
	chain->displayBlockchain();
	system("pause");
}

void main2() {
	std::cout << "Thread created!\n";
	serv = new Server();
}