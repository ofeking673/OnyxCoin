#include "Server.h"

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
}
