#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "Networking/WSAInitializer.h"
#include "DiscoveryServer.h"
#include "P2PNode.h"

#include "IMessageHandler.h"
DiscoveryServer* createDiscoveryServer();

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage
	/// Initializing the basic discovery server
	DiscoveryServer* disc = createDiscoveryServer();
	disc->start("127.0.0.1", 4444);

	P2PNode node(false, "../client1");
	node.start("127.0.0.1", 1234);
	P2PNode node2(false, "../client2");
	node2.start("127.0.0.1", 1235);


	system("pause");
	return 0;
}

DiscoveryServer* createDiscoveryServer() {

	return new DiscoveryServer("../discovery");
}