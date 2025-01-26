#include "Client.h"

std::string Client::signMessage(std::string msg)
{
	ECDSASigner ecd;
	return ecd.signMessage(cpp_int("0x" + wallet->getPrivateKey()), msg)->ToString();
}
