#pragma once
#include "JsonPacketSerializer.h"
#include "Server.h"
class DiscoveryServer
{
public:
	DiscoveryServer();
	static void HandleClient(SOCKET sock);
	static std::string getSocketInfo(SOCKET sock);
	static std::string str();
	static std::pair<std::string, std::string> splitData(std::string data);
private:
	Socket serverSock;
	static std::map<std::string, std::pair<std::string, int>> peers;
};

