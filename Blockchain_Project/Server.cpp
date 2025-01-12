#include "Server.h"

// Define the static members
Socket* Server::serverSock_ = nullptr;
FullNodeMessageHandler* Server::handler = nullptr;
MessageManager* Server::messageManager = nullptr;
/*
MakeTransaction = 100,
Mine = 200,*/

Server::Server(IClient* cli)
{
	_cli = cli;
	handler = new FullNodeMessageHandler();
	messageManager = new MessageManager();
	serverSock_ = new Socket(8026);
	serverSock_->WaitForClients(HandleClient);
}

void Server::HandleClient(SOCKET clientSock)
{
	std::string address = Socket::readFromSock(clientSock);

	try {
		char buf[READ_SIZE];
		while (true) {
			for (int i = 0; i < READ_SIZE; i++)
			{
				buf[i] = 0;
			}
			
			std::string msg = serverSock_->readFromSock(clientSock);
			json j = JsonPacketDeserializer::DeserializeRequest(msg);
			std::string response;
			std::cout << j.at("status").get<Requests>() << std::endl;
			switch (j.at("status").get<Requests>())
			{
			
			default:
				std::cout << "Message code does not exist! {" << j["status"] << "}\n";
				break;
			}
			serverSock_->sendMessage(clientSock, response);
		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
	closesocket(clientSock);
}
