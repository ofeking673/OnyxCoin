#include "Blockchain.h"

int main()
{
	Blockchain myBlockchain;

	Transaction tx1("Alice", "Bob", 50);
	Transaction tx2("Bob", "Charlie", 25);

	myBlockchain.addTransaction(tx1);
	myBlockchain.addTransaction(tx2);

	myBlockchain.minePendingTransaction("Miner1");
	
	myBlockchain.displayBlockchain();

	if (myBlockchain.isChainValid())
	{
		std::cout << "Blockchain is valid" << std::endl;
	}
	else
	{
		std::cout << "Blockchain is invalid" << std::endl;
	}
}