#pragma once
#include "JsonPacketSerializer.h"
#include "Server.h"
class DiscoveryServer
{
public:
	DiscoveryServer();
	static void HandleClient(SOCKET sock);
	static std::pair<std::string, int> getSocketInfo(SOCKET sock);
	static std::string str();
private:
	static Socket serverSock;
	static std::map<std::string, std::pair<std::string, int>> peers;
};

