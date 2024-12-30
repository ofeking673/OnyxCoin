#pragma once
#include "RequestHandler.h"
#include <map>

class Server {
public:
	Server();
	static void HandleClient(SOCKET clientSock);
	SOCKET findByKey(std::string& k) { return SOCKET(); }; // TO-DO: Real Implementation
private:
	static std::map<SOCKET, IClient*> Users_;
	static Socket* serverSock_;
	static RequestHandler* handler;
};
