#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "Networking/WSAInitializer.h"
#include "DiscoveryServer.h"
#include "P2PNode.h"

void main2();

int main()
{
	WSAInitializer wsa; //initialize WSA for socket usage
	P2PNode p2p1("../client1", 1234);
	P2PNode p2p2("../client2", 1235);

	p2p1.pingAll();

	system("pause");
	return 0;
}

void main2() 
{
	DiscoveryServer* disc = new DiscoveryServer();
}