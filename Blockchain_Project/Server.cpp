#include "Server.h"
/*
MakeTransaction = 100,
Mine = 200,*/

Server::Server()
{
	serverSock_ = new Socket(8026);
	serverSock_->WaitForClients(HandleClient);
}

void Server::HandleClient(SOCKET clientSock)
{
	std::string pubKey = Socket::readFromSock(clientSock);
	Users_[clientSock] = pubKey;

	try {
		char buf[READ_SIZE];
		while (true) {
			for (int i = 0; i < READ_SIZE; i++)
			{
				buf[i] = 0;
			}
			


		}
	} 
	catch(std::exception& e) 
	{
		std::cout << __FUNCTION__ ": " << e.what() << std::endl;
	}
}

void Server::mine(SOCKET clientSock, std::string pubKey)
{
	//Get the nonce and new hash from sock



	//Blockchain* chain = Blockchain::getInstance();
	////placeholder mining algorithm, will probably be configured and changed later.
	//int nonce = 0;
	//std::string data = chain->getCurrentBlockInfo();
	//std::string hash = Blockchain::sha->digest(data);
	
}

