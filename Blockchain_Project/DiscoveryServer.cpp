#include "DiscoveryServer.h"
Socket DiscoveryServer::serverSock(4444);

DiscoveryServer::DiscoveryServer()
{
	serverSock.WaitForClients(DiscoveryServer::HandleClient);
}

void DiscoveryServer::HandleClient(SOCKET sock) 
{
    std::string pubKey = serverSock.readFromSock(sock);
    peers[pubKey] = getSocketInfo(sock);

    std::string peerString = str();
    serverSock.sendMessage(sock, peerString);
    closesocket(sock);
}

std::pair<std::string, int> DiscoveryServer::getSocketInfo(SOCKET sock)
{
    sockaddr_in addr;
    int addrLen = sizeof(addr);

    // Get remote address (peer)
    //if (getpeername(sock, (sockaddr*)&addr, &addrLen) == 0) {
    //    std::cout << "Remote IP: " << inet_ntoa(addr.sin_addr)
    //        << ", Remote Port: " << ntohs(addr.sin_port) << std::endl;
    //}
    //else {
    //    std::cerr << "getpeername failed: " << WSAGetLastError() << std::endl;
    //}

    // Get local address
    if (getsockname(sock, (sockaddr*)&addr, &addrLen) == 0) {
        std::cout << "Local IP: " << inet_ntoa(addr.sin_addr)
            << ", Local Port: " << ntohs(addr.sin_port) << std::endl;
    }
    else {
        std::cerr << "getsockname failed: " << WSAGetLastError() << std::endl;
    }
    return { inet_ntoa(addr.sin_addr) , ntohs(addr.sin_port) };
}

std::string DiscoveryServer::str()
{
    std::string ss;
    ss += "{";
    for (const auto& [key, value] : peers) 
    {
        ss += key + ": " + "[" + value.first + ',' + std::to_string(value.second) + "], ";
    }

    if (!peers.empty()) { ss.pop_back(); ss.pop_back(); }

    ss += "}";
    return ss;
}
