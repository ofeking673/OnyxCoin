#pragma once
#include <WinSock2.h>
#include <thread>
#include <iostream>
#include "../json.hpp"
#include "../Blockchain.h"
#include "../JsonPacketSerializer.h"

//This is the base include for all socket related classes, So I'm defining everything here.
using json = nlohmann::json;
class Server;

#define READ_SIZE 2048
#define IFACE 0

class Socket
{
public:
	Socket(int destPort);
	void WaitForClients(std::function<void(SOCKET)> handleClient) const;
	static void sendMessage(SOCKET sock, std::string& msg);
	static std::string readFromSock(SOCKET sock);
private:
	SOCKET serverSocket_;
};

