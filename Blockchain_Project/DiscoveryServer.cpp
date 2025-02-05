//#include "DiscoveryServer.h"
//std::map<std::string, std::pair<std::string, int>> DiscoveryServer::peers;
//
//DiscoveryServer::DiscoveryServer() : serverSock(4444)
//{
//	serverSock.WaitForClients(&DiscoveryServer::HandleClient);
//}
//
//void DiscoveryServer::HandleClient(SOCKET sock) 
//{
//    std::string data = Socket::readFromSock(sock);
//    std::string ip = getSocketInfo(sock);
//    auto pair = splitData(data);
//
//    peers[pair.first] = std::make_pair(ip, std::stoi(pair.second));
//
//    std::string peerString = str();
//    std::cout << peerString << std::endl;
//    Socket::sendMessage(sock, peerString);
//    closesocket(sock);
//}
//
//std::string DiscoveryServer::getSocketInfo(SOCKET sock)
//{
//    sockaddr_in addr;
//    int addrLen = sizeof(addr);
//
//    // Get local address
//    if (getpeername(sock, (sockaddr*)&addr, &addrLen) == 0) {
//        std::cout << "Local IP: " << inet_ntoa(addr.sin_addr)
//            << ", Local Port: " << ntohs(addr.sin_port) << std::endl;
//    }
//    else {
//        std::cerr << "getsockname failed: " << WSAGetLastError() << std::endl;
//    }
//    return inet_ntoa(addr.sin_addr);
//}
//
//std::string DiscoveryServer::str()
//{
//    json j;
//    j["test"] = peers;
//
//    std::cout << j["test"].dump();
//    return j["test"].dump();
//}
//
//std::pair<std::string, std::string> DiscoveryServer::splitData(std::string data)
//{
//    std::pair<std::string, std::string> pair;
//    pair.first = data.substr(0, data.find_first_of("|"));
//    pair.second = data.substr(data.find_first_of("|")+1);
//    return pair;
//}



#include "DiscoveryServer.h"


DiscoveryServer::DiscoveryServer(const std::string& filePath)
    : P2PNode(/* isDiscoveryServer = */ true, filePath)
    , m_nextNodeId(0)
{
}

bool DiscoveryServer::start(const std::string& listenAddress, uint16_t port)
{
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET)
    {
        std::cerr << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    // Setup the listening address
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(port);

    // Convert the address from string to binary form
    inet_pton(AF_INET, listenAddress.c_str(), &service.sin_addr);

    // Bind
    int iResult = bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "[Error] bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    // Listen
    iResult = listen(m_listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "[Error] listen() failed: " << WSAGetLastError() << std::endl;
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    // Mark running and launch the accept thread
    m_isRunning = true;
    m_acceptThread = std::thread(&DiscoveryServer::acceptLoop, this);

    std::cout << "[Info] Node is listening on " << listenAddress << ":" << port << std::endl;
    m_myIP = listenAddress;
    m_myPort = port;
    return true;
}

void DiscoveryServer::acceptLoop()
{
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    while (m_isRunning) {
        SOCKET socket = accept(m_listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (socket == INVALID_SOCKET) {
            std::cerr << "[Error] In accept().\n";
            continue;
        }
        std::thread(&DiscoveryServer::handleClient, this, socket).detach();
    }
}

void DiscoveryServer::handleClient(SOCKET socket)
{
    char buf[2048];
    int bytesRecieved = recv(socket, buf, 2048, 0);
    if (bytesRecieved <= 0) {
        std::cerr << "[Error] in recv().\n";
        return;
    }
    std::string res(buf, bytesRecieved);
    MessageP2P message = MessageP2P::fromJson(json::parse(res));
    MessageP2P resp = onDiscoveryMessage(message);
    std::string temp = resp.toJson().dump();
    int result = send(socket, temp.c_str(), temp.size(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "[Error] In send() Error: " << WSAGetLastError() << std::endl;
    }
}

MessageP2P DiscoveryServer::onDiscoveryMessage(const MessageP2P& msg)
{
    // Check type
    if (msg.getType() == MessageType::DISCOVERY_REQUEST)
    {
        // Extract IP/port from incoming message
        std::string ip;
        uint16_t port = 0;
        auto payload = msg.getPayload();
        if (payload.contains(DISCOVERY_PAYLOAD_LISTEN_IP))
        {
            ip = payload[DISCOVERY_PAYLOAD_LISTEN_IP].get<std::string>();
        }
        if (payload.contains(DISCOVERY_PAYLOAD_LISTEN_PORT))
        {
            port = (uint16_t)payload[DISCOVERY_PAYLOAD_LISTEN_PORT].get<int>();
        }

        // Assign a new node ID
        uint64_t assignedId = m_nextNodeId.fetch_add(1);
        std::string assignedIdStr = std::to_string(assignedId);

        // Respond with DISCOVERY_RESPONSE
        MessageP2P response;
        response.setType(MessageType::DISCOVERY_RESPONSE);
        response.setAuthor(this->m_myPublicKey); // server's pubkey

        json respPayload;
        respPayload[DISCOVERY_PAYLOAD_ASSIGNED_ID] = assignedIdStr;

        // Build a list of known nodes to send back
        json nodeListJson = json::array();
        {
            std::lock_guard<std::mutex> lock(m_knownNodesMutex);
            for (auto& n : m_knownNodes)
            {
                json oneNode;
                oneNode["ip"] = n.ip;
                oneNode["port"] = n.port;
                oneNode["publicKey"] = n.publicKey;
                oneNode["nodeId"] = n.nodeId;
                nodeListJson.push_back(oneNode);
            }
        }

        respPayload[DISCOVERY_PAYLOAD_NODE_LIST] = nodeListJson;
        response.setPayload(respPayload);

        // Save new node in the internal list
        {
            std::lock_guard<std::mutex> lock(m_knownNodesMutex);
            PeerInfo info;
            info.ip = ip;
            info.port = port;
            info.publicKey = msg.getAuthor();  // The public key of the request
            info.nodeId = assignedIdStr;
            m_knownNodes.push_back(info);
        }

        // Send response directly back to the requester
        return response;
    }
    // Additional logic if you want to handle DISCOVERY_RESPONSE from other servers, etc.
}
