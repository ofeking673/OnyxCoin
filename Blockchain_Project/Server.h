#pragma once
#include "Networking/Socket.h"
#include "FullNodeMessageHandler.h"
#include "MessageManager.h"
#include "IClient.h"
#include <map>

class Server {
public:
	Server(IClient* cli);
	static void HandleClient(SOCKET clientSock);
private:
	IClient* _cli;
	static Socket* serverSock_;
	static FullNodeMessageHandler* handler;
	static MessageManager* messageManager;
};
