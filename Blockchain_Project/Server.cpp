#include "Server.h"

// Define the static members
std::map<SOCKET, std::string> Server::Users_;
Socket* Server::serverSock_ = nullptr;

/*
MakeTransaction = 100,
Mine = 200,*/

Server::Server()
{
	blockchain = Blockchain::getInstance();
	serverSock_ = new Socket(8026);
	serverSock_->WaitForClients(HandleClient);
}

void Server::HandleClient(SOCKET clientSock)
{
	std::string address = Socket::readFromSock(clientSock);
	Users_[clientSock] = address;

	try {
		char buf[READ_SIZE];
		while (true) {
			for (int i = 0; i < READ_SIZE; i++)
			{
				buf[i] = 0;
			}
			
			std::string msg = serverSock_->readFromSock(clientSock);
			json j = JsonPacketDeserializer::DeserializeRequest(msg);

			switch (j.template get<Requests>())
			{
			case Mine:
				mine(clientSock, address, j);
			case MakeTransaction:
				
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

std::string Server::mine(SOCKET& clientSock, std::string& address, json j)
{
	//Get the nonce and new hash from sock
	auto info = blockchain->getCurrentBlockInfo();
	std::string expectedHash = j["hash"];
	int nonce = j["nonce"];
	std::string hash = Blockchain::sha->digest(info + std::to_string(nonce));
	if (hash == expectedHash) {
		//nonce is correct, mining is solved
		blockchain->addTransaction(Transaction("System", address, 10));
	}
	std::string response = JsonPacketSerializer::serializeMiningResponse((hash == expectedHash), 1);
	return response;
}

SOCKET Server::findByKey(std::string& k)
{
	for (const auto& [socket, pair] : Users_) {
		if (pair.first == k) {
			return socket;
		}
	}
	throw std::runtime_error(__FUNCTION__ ": Did not find socket for key: " + k);
}

