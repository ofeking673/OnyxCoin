#include "Wallet.h"

void Wallet::InitializeTransaction(std::string k2, double amt)
{
	Transaction trans(_k, k2, amt);
	Blockchain* chain = Blockchain::getInstance();
	chain->addTransaction(trans);
}
