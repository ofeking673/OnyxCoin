#pragma once
#include "Networking/Socket.h"
#include "FullNodeMessageHandler.h"
#include "MessageManager.h"
#include "MessageDispatcher.h"
#include "IClient.h"
#include <map>

class Server {
public:
	Server(IClient* cli, int port, FullNodeMessageHandler* handler);
	static void HandleClient(SOCKET clientSock);
private:
	IClient* _cli;
	MessageDispatcher* dispatcher;
	static Socket* serverSock_;
	static MessageManager* messageManager;
};
