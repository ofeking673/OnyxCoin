#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "KeyGenerator.h"
#include "ECDSASigner.h"
#include "Base58.h"
#include "BIP39SeedMaker.h"
#include "Test.h"
#include "AES256CBC.h"

int main()
{
	//Blockchain myBlockchain;

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
	AES256CBC* aes = new AES256CBC("00000000000000000000000000000000");
	std::cout << std::hex << cpp_int(aes->cbcEncrypt("111111111111111", std::vector<int>(16, 0x00)));
}