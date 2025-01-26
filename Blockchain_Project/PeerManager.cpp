#include "PeerManager.h"

void PeerManager::addPeer(std::string ip, int port)
{
	ClientSocket sock(ip, port);
	std::string pubKey = sock.sendAndRecv(_cli.pubKey);
    peers[pubKey] = { sock.getSocket(), true };
}

void PeerManager::discoverPeers(std::string msg)
{
    json j = json::parse(msg);
    std::map<std::string, std::pair<std::string, int>> Temppeers = j.get<std::map<std::string, std::pair<std::string, int>>>();
    auto lastElement = std::prev(Temppeers.end());
    _personalSocketDetails.first = lastElement->second.first;
    _personalSocketDetails.second = lastElement->second.second;

    for (auto& [key, value] : Temppeers) {
        std::cout << "IP: " << value.first << ", Port: " << value.second << std::endl;
        if (value.second != _personalSocketDetails.second)
        {
            addPeer(value.first, value.second);
        }
    }
}

void PeerManager::sendMessage(std::string dst, std::string msg)
{
    SOCKET sock = peers.at(dst).second;
    Socket::sendMessage(sock, msg);
}

std::string PeerManager::recvMessage(std::string src)
{
    auto sockPair = peers.at(src);
    if (sockPair.second == false) { peers[src] = { sockPair.first, true }; }
    return Socket::readFromSock(sockPair.second);
}

std::vector<SOCKET> PeerManager::getAllClients()
{
    std::vector<SOCKET> clients;

    for (const auto& [key, value] : peers) {
        if (value.second)
        {
            clients.push_back(value.first);
        }
    }

    return clients;
}

std::string PeerManager::signMessage(std::string msg)
{
    return _cli.signMessage(msg);
}
