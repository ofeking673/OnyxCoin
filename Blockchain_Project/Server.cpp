#include "Server.h"

// Define the static members
std::map<SOCKET, IClient*> Server::Users_; 
Socket* Server::serverSock_ = nullptr;
RequestHandler* Server::handler = nullptr;

/*
MakeTransaction = 100,
Mine = 200,*/

Server::Server()
{
	handler = RequestHandler::getInstance();
	serverSock_ = new Socket(8026);
	serverSock_->WaitForClients(HandleClient);
}

void Server::HandleClient(SOCKET clientSock)
{
	std::string address = Socket::readFromSock(clientSock);
	Users_[clientSock] = new Client(2086, address);

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
			case Mine:
				response = handler->mine(address, j);
				break;
			case MakeTransaction:
				//std::cout << "pubkey is :" << address << std::endl;
				response = handler->transaction(address, j);
				break;
			default:
				break;
			}
			serverSock_->sendMessage(clientSock, response);

		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
}
