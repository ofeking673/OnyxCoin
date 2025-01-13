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

    if (j.is_array() && !j.empty())
    {
        json lastItem = j.back();
        if (lastItem.is_object())
        {
            for (auto& [key, value] : lastItem.items()) {
                std::cout << "Key: " << key << ", Value: " << value << std::endl;
            }
        }
        else
        {
            std::cout << "The last item is not a dictionary (object)!" << std::endl;
        }
    }
    else
    {
        std::cout << "JSON is not an array or it's empty!" << std::endl;
    }

    for (auto& [key, value] : j.items()) {
        if (key != _personalSocketDetails.first && value != _personalSocketDetails.second)
        {
            addPeer(key, value);
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
