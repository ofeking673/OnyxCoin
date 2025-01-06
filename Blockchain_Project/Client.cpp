#include "Client.h"

void Client::initializeTransaction(std::string dstAddress, double amt)
{
	Transaction tx = wallet->createTransaction(dstAddress, amt);
	if (tx.isTransactionCreationSucceeded())
	{
		std::string msg = JsonPacketSerializer::serializeTransactionRequest(_k, wallet->createTransaction(dstAddress, amt));
		std::string response = sock.sendAndRecv(msg);
		json res = json::parse(response);
		if (res["status"] == MakeTransactionSuccess) {
			std::cout << __FUNCTION__ ": Successfully transferred " << amt << " To " << dstAddress << std::endl;
		}
	}
	else
	{
		std::cout << "Couldn't create transaction in Client" << std::endl;
	}

}
