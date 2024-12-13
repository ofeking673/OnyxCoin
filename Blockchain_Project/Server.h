#pragma once
#include "Networking/Socket.h"
#include <map>
class Server {
public:
	Server();
	static void HandleClient(SOCKET clientSock);
private:
	static std::map<SOCKET, std::string> Users_;
	static Socket* serverSock_;
};