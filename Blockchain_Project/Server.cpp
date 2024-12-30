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
			switch (j.template get<Requests>())
			{
			case Mine:
				response = handler->mine(address, j);
				serverSock_->sendMessage(clientSock, response);
				break;
			case MakeTransaction:
				response = handler->transaction(address, j);
			default:
				break;
			}
		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
}

//add funciton to verify transaction sending and authorization



//SOCKET Server::findByKey(std::string& k)
//{
//	for (const auto& [socket, pair] : Users_) {
//		if (pair.first == k) {
//			return socket;
//		}
//	}
//	throw std::runtime_error(__FUNCTION__ ": Did not find socket for key: " + k);
//}

