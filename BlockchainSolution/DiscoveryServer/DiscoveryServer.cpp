#include "pch.h"

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage
	/// Initializing the basic discovery server
	DiscoveryServer* disc = new DiscoveryServer("../discovery");
	disc->start("127.0.0.1", 4444);
	
	system("pause");
}