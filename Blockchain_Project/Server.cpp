#include "Server.h"
#include "FullNodeMessageHandler.h"
#include "MessageParser.h"

// Define the static members
MessageManager* Server::messageManager = nullptr;

/*
MakeTransaction = 100,
Mine = 200,*/

Server::Server(IClient* cli, int port, IMessageHandler* handler) :
	_dispatcher(new MessageDispatcher(handler))
{
	_cli = cli;
	messageManager = new MessageManager();
	serverSock_ = new Socket(port);
	
	std::thread([this]() {
		serverSock_->WaitForClients([this](SOCKET clientSock) {
			HandleClient(clientSock);
		});
	}).detach();
}

void Server::HandleClient(SOCKET clientSock)
{
	std::string address = Socket::readFromSock(clientSock);
	if (!_dispatcher->isKnownUser(address)) {
		_dispatcher->addPeer(address, clientSock);
	}
	std::string pubKey = this->_cli->pubKey;
	Socket::sendMessage(clientSock, pubKey);
	try {
		char buf[READ_SIZE];
		while (true) {
			for (int i = 0; i < READ_SIZE; i++)
			{
				buf[i] = 0;
			}
			
			std::string strmsg = serverSock_->readFromSock(clientSock);

			MessageP2P msg = MessageParser::parse(strmsg);
			auto messageList = _dispatcher->dispatch(msg);
			// Currently no message is supposed to be over 1 block, so just send the first one.
			_dispatcher->sendClient(address, messageList[0]);

		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
	closesocket(clientSock);
}
