#include "DiscoveryServer.h"
std::map<std::string, std::pair<std::string, int>> DiscoveryServer::peers;

DiscoveryServer::DiscoveryServer() : serverSock(4444)
{
	serverSock.WaitForClients(&DiscoveryServer::HandleClient);
}

void DiscoveryServer::HandleClient(SOCKET sock) 
{
    std::string data = Socket::readFromSock(sock);
    std::string ip = getSocketInfo(sock);
    auto pair = splitData(data);

    peers[pair.first] = std::make_pair(ip, std::stoi(pair.second));

    std::string peerString = str();
    std::cout << peerString << std::endl;
    Socket::sendMessage(sock, peerString);
    closesocket(sock);
}

std::string DiscoveryServer::getSocketInfo(SOCKET sock)
{
    sockaddr_in addr;
    int addrLen = sizeof(addr);

    // Get local address
    if (getpeername(sock, (sockaddr*)&addr, &addrLen) == 0) {
        std::cout << "Local IP: " << inet_ntoa(addr.sin_addr)
            << ", Local Port: " << ntohs(addr.sin_port) << std::endl;
    }
    else {
        std::cerr << "getsockname failed: " << WSAGetLastError() << std::endl;
    }
    return inet_ntoa(addr.sin_addr);
}

std::string DiscoveryServer::str()
{
    std::string ss;
    ss += "{";
    for (const auto& [key, value] : peers) 
    {
        ss += "\"" + key + "\": " + "[\"" + value.first + "\",\"" + std::to_string(value.second) + "\"], ";
    }

    if (!peers.empty()) { ss.pop_back(); ss.pop_back(); }

    ss += "}";
    return ss;
}

std::pair<std::string, std::string> DiscoveryServer::splitData(std::string data)
{
    std::pair<std::string, std::string> pair;
    pair.first = data.substr(0, data.find_first_of("|"));
    pair.second = data.substr(data.find_first_of("|")+1);
    return pair;
}
