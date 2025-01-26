#pragma once
#include "Networking/Socket.h"
#include "MessageManager.h"
#include "MessageDispatcher.h"
#include "IClient.h"
#include <map>


class Server {
public:
	Server(IClient* cli, int port, IMessageHandler* handler);
	void HandleClient(SOCKET clientSock);
private:
	IClient* _cli;
	MessageDispatcher* dispatcher;
	Socket* serverSock_;
	static MessageManager* messageManager;
};
