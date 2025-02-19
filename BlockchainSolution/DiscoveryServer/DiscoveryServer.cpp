#include "pch.h"

DiscoveryServer* createDiscoveryServer();

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage
	/// Initializing the basic discovery server
	DiscoveryServer* disc = createDiscoveryServer();
	disc->start("127.0.0.1", 4444);

	system("pause");
}

DiscoveryServer* createDiscoveryServer() 
{
	return new DiscoveryServer("../discovery");
}