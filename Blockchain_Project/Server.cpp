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
			_dispatcher->dispatch(msg);





			//json j = JsonPacketDeserializer::DeserializeRequest(msg);
			//std::string response;
			//std::cout << j.at("status").get<Requests>() << std::endl;
			//switch (j.at("status").get<Requests>())
			//{
			//
			//default:
			//	std::cout << "Message code does not exist! {" << j["status"] << "}\n";
			//	break;
			//}
			//serverSock_->sendMessage(clientSock, response);
		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
	closesocket(clientSock);
}
