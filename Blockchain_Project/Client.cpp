#include "Client.h"

void Client::initializeTransaction(std::string dstAddress, double amt)
{
	std::string msg = JsonPacketSerializer::serializeMiningRequest(_k, dstAddress, amt);
	
}
