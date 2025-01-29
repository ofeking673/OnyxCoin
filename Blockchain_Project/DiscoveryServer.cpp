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


DiscoveryServer::DiscoveryServer(const std::string& myNodeId, const std::string& myPublicKey)
    : P2PNode(myNodeId, myPublicKey, /* isDiscoveryServer = */ true)
    , m_nextNodeId(0)
{
}

void DiscoveryServer::onDiscoveryMessage(const MessageP2P& msg)
{
    // Check type
    int type = msg.getType();
    if (type == static_cast<int>(ExtendedMessageType::DISCOVERY_REQUEST))
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

        // Save new node in the internal list
        {
            std::lock_guard<std::mutex> lock(m_knownNodesMutex);
            NetworkNodeInfo info;
            info.ip = ip;
            info.port = port;
            info.publicKey = msg.getAuthor();  // The public key of the request
            info.nodeId = assignedIdStr;
            m_knownNodes.push_back(info);
        }

        // Respond with DISCOVERY_RESPONSE
        MessageP2P response;
        response.setType(static_cast<int>(ExtendedMessageType::DISCOVERY_RESPONSE));
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

        // Send response directly back to the requester
        sendMessageTo(response, msg.getAuthor());
    }
    // Additional logic if you want to handle DISCOVERY_RESPONSE from other servers, etc.
}

void DiscoveryServer::handleDiscoveryMessage(const MessageP2P& msg)
{
    // Check type
    int type = msg.getType();
    if (type == static_cast<int>(ExtendedMessageType::DISCOVERY_REQUEST))
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

        // Save new node in the internal list
        {
            std::lock_guard<std::mutex> lock(m_knownNodesMutex);
            NetworkNodeInfo info;
            info.ip = ip;
            info.port = port;
            info.publicKey = msg.getAuthor();  // The public key of the request
            info.nodeId = assignedIdStr;
            m_knownNodes.push_back(info);
        }

        // Respond with DISCOVERY_RESPONSE
        MessageP2P response;
        response.setType(static_cast<int>(ExtendedMessageType::DISCOVERY_RESPONSE));
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

        // Send response directly back to the requester
        sendMessageTo(response, msg.getAuthor());
    }
    // Additional logic if you want to handle DISCOVERY_RESPONSE from other servers, etc.
}
