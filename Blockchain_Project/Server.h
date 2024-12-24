#pragma once
#include "..\Blockchain_Project\Networking\Socket.h"
#include "Client.h"
#include "Miner.h"
#include <map>

class Server {
public:
	Server();
	static void HandleClient(SOCKET clientSock);
	static std::string mine(SOCKET& clientSock, std::string& address, json j);
	SOCKET findByKey(std::string& k);
private:
	static std::map<SOCKET, std::pair<std::string, IClient*>> Users_;
	static Socket* serverSock_;
	static Blockchain* blockchain;
};
