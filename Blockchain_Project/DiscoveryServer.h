#pragma once




//#include "JsonPacketSerializer.h"
//#include "Server.h"
//class DiscoveryServer
//{
//public:
//	DiscoveryServer();
//	static void HandleClient(SOCKET sock);
//	static std::string getSocketInfo(SOCKET sock);
//	static std::string str();
//	static std::pair<std::string, std::string> splitData(std::string data);
//private:
//	Socket serverSock;
//	static std::map<std::string, std::pair<std::string, int>> peers;
//};

#include "P2PNode.h"

class DiscoveryServer : public P2PNode
{
public:
    DiscoveryServer(const std::string& myNodeId,
        const std::string& myPublicKey);

    // The main logic that intercepts discovery messages:
    void onDiscoveryMessage(const MessageP2P& msg);

protected:
    // Here we override the base class's discovery message handler, invoked by receiveLoop
    void handleDiscoveryMessage(const MessageP2P& msg);

private:
    std::atomic<uint64_t> m_nextNodeId;
    std::mutex            m_knownNodesMutex;
    std::vector<NetworkNodeInfo> m_knownNodes;
};
