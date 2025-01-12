#include "PeerManager.h"

void PeerManager::addPeer(std::string ip, int port)
{
	ClientSocket sock(ip, port);
	std::string pubKey = sock.sendAndRecv(_cli.pubKey);
	peers[{pubKey, true}] = sock.getSocket();
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
    SOCKET sock = peers.at({dst, true});
    Socket::sendMessage(sock, msg);
}

std::string PeerManager::recvMessage(std::string src)
{
    try {
        SOCKET sock = peers.at({ src, true });
        return Socket::readFromSock(sock);
    }
    catch (std::exception& e) 
    {

    }
}
