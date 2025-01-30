//#include "P2PNode.h"
//#include "FullNodeMessageHandler.h"
//
//P2PNode::P2PNode(int handlerIdentifier, std::string keyPath, int port)
//{
//	switch (handlerIdentifier)
//	{
//		case(FULL_NODE_MESSAGE_HANDLER):
//			_handler = new FullNodeMessageHandler(keyPath, port);
//			break;
//		default:
//			_handler = nullptr;
//	}
//}
//
//P2PNode::~P2PNode()
//{
//	delete _handler;
//}
//
//void P2PNode::pingAll()
//{
//	MessageP2P pingMsg = _messageCreator.createPingMessage(_handler->getPublicKey());
//	std::string signature = signMessage(pingMsg.toJson().dump());
//	pingMsg.setSignature(signature);
//	std::vector<SOCKET> clients = _handler->getAllClients();
//	for (const auto& client : clients) {
//		std::string msg = pingMsg.toJson().dump();
//		Socket::sendMessage(client, msg);
//	}
//}
//
//std::string P2PNode::signMessage(std::string message)
//{
//	return _handler->signMessage(message);
//}

#include "P2PNode.h"
#include "MessageParser.h"
#include "Messagemanager.h"

P2PNode::P2PNode(const std::string& myNodeId, const std::string& myPublicKey, bool isDiscoveryServer)
    : m_myNodeId(myNodeId)
    , m_myPublicKey(myPublicKey)
    , m_isRunning(false)
    , m_isDiscoveryServer(isDiscoveryServer)
{
}

P2PNode::~P2PNode()
{
    stop();
    WSACleanup();
}

bool P2PNode::start(const std::string& listenAddress, uint16_t port)
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        std::cerr << "[Error] WSAStartup failed: " << iResult << std::endl;
        return false;
    }

    // Create a listening socket
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
    iResult = bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));
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
    m_acceptThread = std::thread(&P2PNode::acceptLoop, this);

    std::cout << "[Info] Node is listening on " << listenAddress << ":" << port << std::endl;
    return true;
}


void P2PNode::stop()
{
    if (!m_isRunning)
        return;

    m_isRunning = false;

    // Close the listening socket if open
    if (m_listenSocket != INVALID_SOCKET)
    {
        shutdown(m_listenSocket, SD_BOTH);
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    // Join the accept thread
    if (m_acceptThread.joinable())
    {
        m_acceptThread.join();
    }

    // Close all peer sockets
    std::lock_guard<std::mutex> lock(m_peerMutex);
    for (auto& kv : m_peers)
    {
        SOCKET s = kv.second.socket;
        if (s != INVALID_SOCKET)
        {
            shutdown(s, SD_BOTH);
            closesocket(s);
        }
    }
    m_peers.clear();
}

bool P2PNode::connectToNode(const std::string& ip, uint16_t port, const std::string& remotePublicKey, const std::string& remoteNodeId)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    int result = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "[Error] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return false;
    }

    // If connected, store peer info
    PeerInfo peer;
    peer.ip = ip;
    peer.port = port;
    peer.publicKey = remotePublicKey;
    peer.nodeId = remoteNodeId;
    peer.socket = sock;
    peer.lastContact = std::chrono::steady_clock::now();

    addPeer(peer);


    // Send a handshake message to the node, to notify you established a connection
    PeerInfo myPeerInfo(m_myIP, m_myPort, m_myPublicKey, m_myNodeId);
    MessageP2P handshakeMsg = MessageManager::createHandshakeMessage(m_myPublicKey, myPeerInfo);

    sendMessageTo(handshakeMsg, remotePublicKey);


    // Launch a receive thread for this peer
    std::thread(&P2PNode::receiveLoop, this, sock, remotePublicKey).detach();

    std::cout << "[Info] Connected to node: " << remotePublicKey << " at " << ip << ":" << port << std::endl;
    return true;
}


bool P2PNode::sendMessageTo(MessageP2P& msg, const std::string toPublicKey)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    auto it = m_peers.find(toPublicKey);
    if (it == m_peers.end())
    {
        std::cerr << "[Warning] Peer not found: " << toPublicKey << std::endl;
        return false;
    }

    SOCKET sock = it->second.socket;
    if (sock == INVALID_SOCKET)
        return false;

    // Serialize message
    signMessage(msg);
    json j = msg.toJson();
    std::string wire = j.dump();

    int result = send(sock, wire.c_str(), static_cast<int>(wire.size()), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "[Error] send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}


void P2PNode::broadcastMessage(MessageP2P& msg)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    for (auto& peer : m_peers)
    {
        sendMessageTo(msg, peer.second.publicKey);
    }
}

void P2PNode::addPeer(const PeerInfo& newPeer)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    m_peers[newPeer.publicKey] = newPeer;
}

void P2PNode::updatePeersLastContact(const std::string& peerPublicKey)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    auto it = m_peers.find(peerPublicKey);
    if (it != m_peers.end())
    {
        it->second.lastContact = std::chrono::steady_clock::now();
    }
}

std::string P2PNode::getMyPublicKey() const
{
    return m_myPublicKey;
}

void P2PNode::acceptLoop()
{
    while (m_isRunning)
    {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(m_listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            if (m_isRunning)
            {
                std::cerr << "[Error] accept() failed: " << WSAGetLastError() << std::endl;
            }
            continue;
        }

        // After accepted, the other peer should send a handshake message

        // Recieve the handshake message, before starting the recieve loop

        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];

        while (m_isRunning)
        {
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesReceived > 0)
            {
                // Get the handshake message
                std::string data(buffer, bytesReceived);

                MessageP2P msg = MessageParser::parse(data);

                if (msg.getType() == MessageType::HANDSHAKE)
                {
                    // Handle the incoming handshake message.
                    // Handler adds the peer to the peer list
                    // Returns a vector of messages to send back (PEER_LIST).
                    std::vector<MessageP2P> responses = m_dispatcher.dispatch(msg);


                    // Find the new peer public key:
                    PeerInfo newPeer = PeerInfo::fromJson(msg.getPayload());
                    std::string remotePublicKey = newPeer.publicKey;


                    // Send back the response messages
                    for (auto& respMsg : responses)
                    {
                        sendMessageTo(respMsg, msg.getAuthor());
                    }

                    std::cout << "[Info] Accepted connection from a peer (socket=" << clientSocket << ")" << std::endl;

                    // Start a thread to receive messages from this new peer
                    std::thread(&P2PNode::receiveLoop, this, clientSocket, remotePublicKey).detach();
                }
            }
    }
}


void P2PNode::receiveLoop(SOCKET sock, const std::string& peerPublicKey)
{
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    while (m_isRunning)
    {
        int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0)
        {
            std::cout << "[Info] Connection closed by peer: " << peerPublicKey << std::endl;
            break;
        }
        else if (bytesReceived < 0)
        {
            // Error or maybe the peer shut down
            int err = WSAGetLastError();
            if (err == WSAECONNRESET)
            {
                std::cerr << "[Warning] Connection reset by peer: " << peerPublicKey << std::endl;
            }
            else
            {
                std::cerr << "[Error] recv() failed for peer " << peerPublicKey << ": " << err << std::endl;
            }
            break;
        }
        else
        {
            // We have data to process
            std::string data(buffer, bytesReceived);
            // In real code, you might have to handle partial messages or multiple messages in one recv.

            MessageP2P msg = MessageParser::parse(data);

            if (msg.getType() != MessageType::ERROR_MESSAGE)
            {
                // Handle the incoming message. Returns a vector of messages to send back.
                std::vector<MessageP2P> responses = m_dispatcher.dispatch(msg);

                // Send back the response messages
                for (auto& respMsg : responses)
                {
                    sendMessageTo(respMsg, msg.getAuthor());
                }
            }
        }
    }

    // Cleanup on exit
    std::lock_guard<std::mutex> lock(m_peerMutex);
    closesocket(sock);
    m_peers.erase(peerPublicKey);
}

void P2PNode::signMessage(MessageP2P& msg)
{
    ECDSASigner ecd;
    msg.setSignature(""); // Clear signature if there is any; signing the message without the signature
    msg.setSignature(ecd.signMessage(cpp_int("0x" + m_myPrivateKey), msg.toJson().dump())->ToString());
}

json P2PNode::peersToJson()
{
    // If the map is modified concurrently, lock it as needed
    std::lock_guard<std::mutex> lock(m_peerMutex);

    // We'll store all PeerInfo objects in a JSON array
    json j = json::array();
    for (const auto& kv : m_peers)
    {
        const PeerInfo& peerInfo = kv.second;
        j.push_back(peerInfo.toJson());
    }
    return j;
}

std::unordered_map<std::string, PeerInfo> P2PNode::fromJsonToPeers(const json& data)
{
    std::unordered_map<std::string, PeerInfo> result;

    if (!data.is_array())
    {
        // Return an empty result if it's not an array.
        return result;
    }

    for (const auto& item : data)
    {
        PeerInfo p = PeerInfo::fromJson(item);
        result[p.publicKey] = p;
    }

    return result;
}

std::unordered_map<std::string, PeerInfo> P2PNode::getNewPeers(const std::unordered_map<std::string, PeerInfo>& newPeers)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);

    std::unordered_map<std::string, PeerInfo> result;
    for (const auto& kv : newPeers)
    {
        const std::string& newPeerPublicKey = kv.first;
        const PeerInfo& newPeerInfo = kv.second;

        // If this publicKey does not exist in the owned map, it's new
        if (m_peers.find(newPeerPublicKey) == m_peers.end())
        {
            result[newPeerPublicKey] = newPeerInfo;
        }
    }
    return result;
}
