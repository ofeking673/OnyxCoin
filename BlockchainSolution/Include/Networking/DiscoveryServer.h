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
    DiscoveryServer(const std::string& filePath);

    virtual bool start(const std::string& listenAddress, uint16_t port) override;

    virtual void acceptLoop() override;
    void handleClient(SOCKET socket);
    // The main logic that intercepts discovery messages:
    MessageP2P onDiscoveryMessage(const MessageP2P& msg);

protected:
    // Here we override the base class's discovery message handler, invoked by receiveLoop
private:
    std::atomic<uint64_t> m_nextNodeId;
    std::mutex            m_knownNodesMutex;
    std::vector<PeerInfo> m_knownNodes;
};
