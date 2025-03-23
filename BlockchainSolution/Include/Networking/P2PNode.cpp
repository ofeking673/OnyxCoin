#include "P2PNode.h"
#include "P2PNode.h"
#include "pch.h"
#include "P2PNode.h"
#include "Messages/MessageParser.h"
#include "Messages/MessageManager.h"
#include "Messages/FullNodeMessageHandler.h"

P2PNode::P2PNode(bool isDiscoveryServer, const std::string& filePath) : 
      m_isRunning(false)
    , m_isDiscoveryServer(isDiscoveryServer)
    , m_dispatcher(this, false)
    , m_discoveryServerPort(DISCOVERY_SERVER_PORT)
    , m_discoveryServerIp(LOCALHOST)
    , m_myWallet(filePath)
    , m_stopMining(false)
{
    m_myPublicKey = m_myWallet.getPublicKey();
    m_myPrivateKey = m_myWallet.getPrivateKey();
}

P2PNode::P2PNode(const std::string& seed) :
    m_isRunning(false)
    , m_isDiscoveryServer(false)
    , m_dispatcher(this, false)
    , m_discoveryServerPort(DISCOVERY_SERVER_PORT)
    , m_discoveryServerIp(LOCALHOST)
    , m_myWallet(seed, true)
    , m_stopMining(false)
{
    m_myPublicKey = m_myWallet.getPublicKey();
    m_myPrivateKey = m_myWallet.getPrivateKey();
}

P2PNode::~P2PNode()
{
    stop();
    WSACleanup();
}

bool P2PNode::start(const std::string& listenAddress, uint16_t port)
{
    m_myIP = listenAddress;
    m_myPort = port;

    // Create a listening socket
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
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
        std::cerr << "{" << m_myPort << "} " << "[Error] bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    // Listen
    iResult = listen(m_listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] listen() failed: " << WSAGetLastError() << std::endl;
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    // Mark running and launch the accept thread
    m_isRunning = true;
    m_acceptThread = std::thread(&P2PNode::acceptLoop, this);

    // Get peers from the discovery server, and intitate connection.
    if (!this->m_isDiscoveryServer) { getPeers(); }

    // Start ping thread
    startPingThread();

    std::cout << "{" << m_myPort << "} " << "[Info] Node is listening on " << listenAddress << ":" << port << std::endl;

    // Ask and recieve the blockchain instance
    recieveBlockchain();

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

    m_pingThread.join();
}

bool P2PNode::connectToNode(const std::string& ip, uint16_t port, const std::string& remotePublicKey, const uint64_t& remoteNodeId)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    int result = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return false;
    }


    // Send a handshake message to the node, to notify you established a connection
    PeerInfo myPeerInfo(m_myIP, m_myPort, m_myPublicKey, m_myNodeId);
    {
        std::lock_guard<std::mutex> lock(m_peerMutex);
        m_peers[remotePublicKey].socket = sock;
    }
    MessageP2P handshakeMsg = MessageManager::createHandshakeMessage(m_myPublicKey, myPeerInfo);

    sendMessageTo(handshakeMsg, remotePublicKey);


    // Launch a receive thread for this peer
    std::thread(&P2PNode::receiveLoop, this, sock, remotePublicKey).detach();

    std::cout << "{" << m_myPort << "} " << "[Info] Connected to node: " << remotePublicKey << " at " << ip << ":" << port << std::endl;
    return true;
}

void P2PNode::connectToPeer(PeerInfo& info)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "{" << m_myPort << "} " << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(info.port);
    inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);

    int result = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return;
    }

    MessageP2P msg = MessageManager::createHandshakeMessage(m_myPublicKey, PeerInfo(m_myIP, m_myPort, m_myPublicKey, m_myNodeId));
    signMessage(msg);

    std::string data = msg.toJson().dump();
    result = send(sock, data.c_str(), data.length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "{" << m_myPort << "} " << "[Error] send() failed: " << WSAGetLastError() << std::endl;
        return;
    }

    // Recieve the handshake message from the other peer
    char buf[2048];
    result = recv(sock, buf, 2048, 0);
    if (result <= 0) {
        std::cerr << "{" << m_myPort << "} " << "[Error] recv() failed!\n";
        return;
    }


    msg = MessageParser::parse(std::string(buf, result));

    if (msg.getAuthor() != info.publicKey)
    {
        // Public key doesn't match the peers public key
        removePeer(info);
        std::cerr << "{" << m_myPort << "} " << "[Error] recieve second Handshake failed!\n";
        return;
    }

    // Add the peer to the peers map
    info.socket = sock;
    addPeer(info);


    // Launch a receive thread for this peer
    std::thread(&P2PNode::receiveLoop, this, sock, info.publicKey).detach();

    std::cout << "{" << m_myPort << "} " << "[Info] Connected to node: " << info.publicKey << " at " << info.ip << ":" << info.port << std::endl;
}

void P2PNode::getPeers()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] socket() failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DISCOVERY_SERVER_PORT);
    inet_pton(AF_INET, LOCALHOST, &addr.sin_addr);

    int result = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] connect() failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return;
    }

    sendDiscovery(sock);

    char buf[2048];
    int bytesRecieved = recv(sock, buf, 2048, 0);
    if (bytesRecieved <= 0) {
        std::cerr << "{" << m_myPort << "} " << "[Error] recv() failed.\n";
        return;
    }
    std::string res(buf, bytesRecieved);
    MessageP2P msg = MessageP2P::fromJson(json::parse(res));
    json payload = msg.getPayload();
    m_myNodeId = payload[DISCOVERY_PAYLOAD_ASSIGNED_ID].get<uint64_t>();

    auto nodes = payload[DISCOVERY_PAYLOAD_NODE_LIST];
    for (const auto& node : nodes) {
        /* oneNode["ip"] = n.ip;
           oneNode["port"] = n.port;
           oneNode["publicKey"] = n.publicKey;
           oneNode["nodeId"] = n.nodeId;
        */
        PeerInfo peer(node["ip"], node["port"], node["publicKey"], node["nodeId"]);
        //addPeer(peer);
        // Connecting to peer, and adding it to the peers map
        connectToPeer(peer);
    }
}

void P2PNode::sendDiscovery(SOCKET sock)
{
    PeerInfo peer(m_myIP, m_myPort, m_myPublicKey, m_myNodeId);
    MessageP2P msg = MessageManager::createDiscoveryRequestMessage(m_myPublicKey, peer);

    signMessage(msg);
    json j = msg.toJson();
    std::string wire = j.dump();
    int result = send(sock, wire.c_str(), wire.size(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "{" << m_myPort << "} " << "[Error] send() failed: " << WSAGetLastError() << std::endl;
    }
}
void P2PNode::recieveBlockchain()
{
    if (amILeader())
    {
        std::lock_guard<std::mutex> lock(m_peerMutex);
        // Create instance of blockchain
        m_dispatcher = MessageDispatcher(this, true);
        // Indicate this is the finalize blockchain and from now on the blockchain is singleton
        m_dispatcher.setBlockchainSingleton();
        return;
    }
    // Indicate this is the finalize blockchain and from now on the blockchain is singleton
    m_dispatcher.setBlockchainSingleton();

    std::vector<std::pair<std::string, std::string>> blockHashes;
    MessageP2P getHeadersMsg = MessageManager::createGetHeadersMessage(getMyPublicKey(), blockHashes, "");
    
    // Send get headers message to leader
    std::string leaderPublicKey = getPeerInfoByID(getLeaderIndex()).publicKey;
    sendMessageTo(getHeadersMsg, leaderPublicKey);
}

std::vector<std::string> P2PNode::getUTXOs()
{
    return m_myWallet.getTransactions();
}


bool P2PNode::sendMessageTo(MessageP2P& msg, const std::string toPublicKey)
{
    if (this == nullptr) {
        std::cerr << "[Critical] *this* is NULL!" << std::endl;
        return false;
    }
    std::lock_guard<std::mutex> lock(m_peerMutex);
    auto it = m_peers.find(toPublicKey);
    if (it == m_peers.end())
    {
        std::cerr << "{" << m_myPort << "} " << "[Warning] Peer not found: " << toPublicKey << std::endl;
        return false;
    }


    SOCKET sock = it->second.socket;
    if (sock == INVALID_SOCKET)
        return false;

    // Serialize message
    signMessage(msg);
    json j = msg.toJson();
    std::string wire = j.dump();

    int result = send(sock, wire.c_str(), wire.size(), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "{" << m_myPort << "} " << "[Error] send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}


void P2PNode::broadcastMessage(MessageP2P& msg)
{
    std::cout << "[Info] Starting broadcast\n";
    for (auto& peer : m_peers)
    {
        sendMessageTo(msg, peer.second.publicKey);
    }
}

bool P2PNode::createTransaction(const std::string& dst, int amount)
{
    Transaction tx = m_myWallet.createTransaction(dst, amount);
    if (tx.getInputs().empty() && tx.getOutputs().empty())
    {
        return false;
    }
    // Create a P2P message wrapping the transaction
    MessageP2P newTxMessage = MessageManager::createNewTransactionMessage(m_myWallet.getPublicKey(), tx);
    broadcastMessage(newTxMessage);
    return true;
}

void P2PNode::leaderUptime()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(45));
        if (!m_leaderActive) {
            //TODO: Call new change
            BlockHeader lastStableBlock = m_dispatcher.getChain()->getLatestBlock().getBlockHeader();
            MessageManager::createViewChange(m_myPublicKey,
                m_currentView,
                lastStableBlock.getIndex(),
                lastStableBlock.getHash());
        }
        m_leaderActive = false;
    }
}

void P2PNode::refreshLeaderUptime()
{
    m_leaderActive = true;
}

PeerInfo P2PNode::getPeerInfoByID(const uint32_t& ID)
{
    if (ID == m_myNodeId) return PeerInfo(m_myIP, m_myPort, m_myPublicKey, m_myNodeId);
    std::lock_guard<std::mutex> lock(m_peerMutex);
    std::cout << "Iterating over peers!\n";
    for (const auto& [key, v] : m_peers)
    {
        if (v.nodeId == ID) {
            std::cout << "Peer found!\n";
            return v;
        }
    }
    throw std::runtime_error("[Error] No peer found with that ID!\n");
}

void P2PNode::addPeer(const PeerInfo& newPeer)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    m_peers[newPeer.publicKey] = newPeer;
}

bool P2PNode::removePeer(const PeerInfo& peer)
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    auto it = m_peers.find(peer.publicKey);
    if (it != m_peers.end())
    {
        m_peers.erase(peer.publicKey);
        return true;
    }
    return false;
}

uint32_t P2PNode::getPeerAmount()
{
    return m_peers.size();
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

void P2PNode::startPingThread()
{
    // Launch a background thread that periodically pings inactive peers
    m_pingThread = std::thread(&P2PNode::pingInactivePeers, this);
}

std::string P2PNode::getMyPublicKey() const
{
    return m_myPublicKey;
}

uint16_t P2PNode::getMyPort() const
{
    return m_myPort;
}

PeerInfo P2PNode::getMyInfo() const
{
    return PeerInfo(m_myIP, m_myPort, m_myPublicKey, m_myNodeId);
}

Block P2PNode::getLastBlock() const
{
    return m_dispatcher.getChain()->getLatestBlock();
}

const Blockchain* P2PNode::getBlockchain() const
{
    return m_dispatcher.getChain();
}

std::vector<PeerInfo> P2PNode::getAllClients()
{
    std::vector<PeerInfo> vec;
    for (const auto& kv : this->m_peers) {
        vec.push_back(kv.second);
    }
    return vec;
}

uint32_t P2PNode::getCurrentView() const
{
    return m_currentView;
}

void P2PNode::setCurrentView(uint32_t newView)
{
    m_currentView = newView;
}

void P2PNode::addNewPhaseState(uint32_t view, int sequence, const Block& block)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    PhaseState phase(block);
    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    m_phaseStates[viewSeq] = phase;
}

bool P2PNode::isTrackingTheState(uint32_t view, int sequence) const
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        return true;
    }
    return false;
}

void P2PNode::addPrepareMessage(uint32_t view, int sequence, const MessageP2P& prepareMessage)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        // Add the prepare message to the phase state container
        it->second.addPrepareMessage(prepareMessage);
    }
}

void P2PNode::addCommitMessage(uint32_t view, int sequence, const MessageP2P& commitMessage)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        // Add the commit message to the phase state container
        it->second.addPrepareMessage(commitMessage);
    }
}

int P2PNode::getPrepareAmount(uint32_t view, int sequence)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        // Get the amount of recieved prepared messages for this (view, sequence)
        return it->second.getPrepareAmount();
    }
    return 0;
}

int P2PNode::getCommitAmount(uint32_t view, int sequence)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        // Get the amount of recieved commit messages for this (view, sequence)
        return it->second.getCommitAmount();
    }
    return 0;
}

void P2PNode::setHashReady(uint32_t view, int sequence, const Block& block)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        it->second.setHashReady(block);
    }
}

void P2PNode::setPrepared(uint32_t view, int sequence)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        it->second.setPrepared();
    }
}

void P2PNode::setCommitted(uint32_t view, int sequence)
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        it->second.setCommitted();
    }
}

bool P2PNode::isHashReady(uint32_t view, int sequence) const
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        return it->second.isHashReady();
    }
    return false;
}

bool P2PNode::isPrepared(uint32_t view, int sequence) const
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        return it->second.isPrepared();
    }
    return false;
}

bool P2PNode::isCommitted(uint32_t view, int sequence) const
{
    std::lock_guard<std::recursive_mutex> lock(m_phaseStatesMutex);

    std::pair<uint32_t, int> viewSeq = std::pair<uint32_t, int>(view, sequence);
    auto it = m_phaseStates.find(viewSeq);
    if (it != m_phaseStates.end())
    {
        return it->second.isCommitted();
    }
    return false;
}

void P2PNode::addViewChangeMessage(uint32_t newView, MessageP2P viewChangeMessage)
{
    if (viewChangeMessage.getType() == MessageType::VIEW_CHANGE)
    {
        // Check if already pushed a message from that author.
        if (!isRecievedViewChangeMessageFromAuthor(newView, viewChangeMessage.getAuthor()))
        {
            auto it = m_viewChangeStates.find(newView);
            if (it == m_viewChangeStates.end())
            {
                // Create a new pair of newView to vector of viewChangeMessages
                std::vector<MessageP2P> viewChanges;
                viewChanges.push_back(viewChangeMessage);
                m_viewChangeStates[newView] = viewChanges;
            }
            else
            {
                // Add the view change message to the tracker
                it->second.push_back(viewChangeMessage);
            }
        }
    }


}

bool P2PNode::isRecievedViewChangeMessageFromAuthor(uint32_t newView, const std::string& author)
{
    auto it = m_viewChangeStates.find(newView);
    if (it != m_viewChangeStates.end())
    {
        // Check if already recieved a view change message from that author (about the current view change)
        for (MessageP2P msg : it->second)
        {
            if (msg.getAuthor() == author)
            {
                return true;
            }
        }
    }

    return false;
}

bool P2PNode::checkRemoteViewChangeMessagesVector(std::vector<MessageP2P> viewChangeMessages)
{
    uint32_t view = m_currentView - 1;
    for (auto& vcmsg : viewChangeMessages)
    {
        uint32_t newView = 0;
        int lastStableSeq = 0;
        std::string checkpointDigest;
        MessageParser::parseViewChangeMessage(vcmsg, newView, lastStableSeq, checkpointDigest);
        
        // TO-DO: Verify signature
        // and check that the signature isn't being repeated


        // Check view
        if (newView != view)
        {
            return false;
        }
    }
    // Verified the messages
    return true;
}

uint32_t P2PNode::getRemoteViewChangeMessageSize(uint32_t view)
{
    return m_viewChangeStates[view].size();
}

uint32_t P2PNode::getLeaderIndex()
{
    if (m_peers.size() == 0) return 0;
    uint32_t leaderIndex = m_currentView % m_peers.size();
    return leaderIndex;
}

bool P2PNode::amILeader()
{
    return (getLeaderIndex() == m_myNodeId);
}


void P2PNode::acceptLoop()
{
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
        // After accepted, the other peer should send a handshake message

        // Recieve the handshake message, before starting the recieve loop

    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    while (m_isRunning)
    {
        SOCKET clientSocket = accept(m_listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            if (m_isRunning)
            {
                std::cerr << "{" << m_myPort << "} " << "[Error] accept() failed: " << WSAGetLastError() << std::endl;
            }
            continue;
        }

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


                // Add the new peer
                PeerInfo newPeer = PeerInfo::fromJson(msg.getPayload());
                // Add the socket connected to the new peer
                newPeer.socket = clientSocket;

                addPeer(newPeer);


                std::string remotePublicKey = newPeer.publicKey;


                // Send back the response messages
                for (auto& respMsg : responses)
                {
                    sendMessageTo(respMsg, msg.getAuthor());
                }

                std::cout << "{" << m_myPort << "} " << "[Info] Accepted connection from a peer (socket=" << clientSocket << ")" << std::endl;

                // Start a thread to receive messages from this new peer
                std::thread(&P2PNode::receiveLoop, this, clientSocket, remotePublicKey).detach();
            }
        }
    }
}


void P2PNode::receiveLoop(SOCKET sock, const std::string& peerPublicKey)
{
    const int BUFFER_SIZE = 10000;
    char buffer[BUFFER_SIZE];

    while (m_isRunning)
    {
        int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == 0)
        {
            std::cout << "{" << m_myPort << "} " << "[Info] Connection closed by peer: " << peerPublicKey << std::endl;
            break;
        }
        else if (bytesReceived < 0)
        {
            // Error or maybe the peer shut down
            int err = WSAGetLastError();
            if (err == WSAECONNRESET)
            {
                std::cerr << "{" << m_myPort << "} " << "[Warning] Connection reset by peer: " << peerPublicKey << std::endl;
            }
            else
            {
                std::cerr << "{" << m_myPort << "} " << "[Error] recv() failed for peer " << peerPublicKey << ": " << err << std::endl;
            }
            break;
        }
        else
        {
            // We have data to process
            std::string data(buffer, bytesReceived);
            // In real code, you might have to handle partial messages or multiple messages in one recv.
            std::cout << "[Info] Recieved new message! " << data << std::endl;
            MessageP2P msg = MessageParser::parse(data);

            if (msg.getType() == MessageType::HANDSHAKE) 
            {
                std::vector<MessageP2P> responses = m_dispatcher.dispatch(msg);
                

                // Send back the response messages
                for (auto& respMsg : responses)
                {
                    sendMessageTo(respMsg, msg.getAuthor());
                }
            }
            else if (msg.getType() != MessageType::ERROR_MESSAGE)
            {
                {
                    std::lock_guard<std::mutex> lock(m_peerMutex);
                    PeerInfo peer = m_peers[msg.getAuthor()];

                    // Verify signature of sender
                    if (!verifySignature(msg))
                    {
                        std::cout << "[Warning] Wrong signature for message" << std::endl;
                        continue;
                    }

                    // Refresh leader up time
                    if (peer.nodeId == getLeaderIndex()) refreshLeaderUptime();
                }
                // Handle the incoming message. Returns a vector of messages to send back.
                std::vector<MessageP2P> responses = m_dispatcher.dispatch(msg);

                // If should broadcast the returned messages
                if (msg.getType() == MessageType::PREPREPARE
                    || msg.getType() == MessageType::HASH_READY
                    || msg.getType() == MessageType::PREPARE
                    || msg.getType() == MessageType::COMMIT
                    || msg.getType() == MessageType::VIEW_CHANGE)
                {
                    for (auto& respMsg : responses)
                    {
                        broadcastMessage(respMsg);
                    }
                }
                else
                {
                    // Send back the response messages
                    for (auto& respMsg : responses)
                    {
                        sendMessageTo(respMsg, msg.getAuthor());
                    }
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
    //msg.setSignature(ecd.signMessage(cpp_int("0x" + m_myPrivateKey), msg.toJson().dump())->ToString());
    Point* pointSignature = ecd.signMessage(cpp_int("0x" + m_myPrivateKey), msg.toJson().dump());
    std::string hexSignature = Point::usePointToHex(pointSignature);
    msg.setSignature(hexSignature);

    delete pointSignature;
}

bool P2PNode::verifySignature(const MessageP2P& msg)
{
    std::string signature = msg.getSignature();
    std::string publicKey = msg.getAuthor();

    MessageP2P checkMsg(msg);
    checkMsg.setSignature("");

    // ECDSA verify:
    //    Rebuild signature Point from signatureHex
    Point* rs = Point::useHexToPoint(signature);
    if (!rs)
    {
        std::cerr << "Invalid signature format in input " << "\n";
        return false;
    }

    //    Rebuild public key Point from publicKey
    Point* pubKeyPoint = Point::useHexToPoint(publicKey);
    if (!pubKeyPoint)
    {
        std::cerr << "Invalid public key format in input " << "\n";
        delete rs;
        return false;
    }

    //std::cout << "[Verify] Public key: " << pubKeyPoint->_x << ":" << pubKeyPoint->_y << std::endl;
    //std::cout << "[Verify] Signature: " << rs->_x << ":" << rs->_y << std::endl;

    //    Verify ECDSA
    ECDSASigner signer;
    bool verified = signer.verifySignature(rs, SHA256::digest(checkMsg.toJson().dump()), pubKeyPoint);

    // Cleanup
    delete rs;
    delete pubKeyPoint;

    if (!verified)
    {
        std::cerr << "Signature verification failed in input " << "\n";
        return false;
    }

    return true;
}

void P2PNode::pingInactivePeers()
{
    const auto inactivityThreshold = std::chrono::seconds(30);
    const auto checkInterval = std::chrono::seconds(5);

    while (m_isRunning)
    {
        // Collect peers we need to ping.
        std::vector<std::string> peersToPing; // Store their public keys
        {
            std::lock_guard<std::mutex> lock(m_peerMutex);
            auto now = std::chrono::steady_clock::now();
            for (auto& kv : m_peers)
            {
                PeerInfo& peer = kv.second;
                auto elapsed = now - peer.lastContact;

                if (elapsed > inactivityThreshold)
                {
                    // Record this peer's public key to ping AFTER we release the lock
                    peersToPing.push_back(peer.publicKey);
                }
            }
        } // Lock is released

        // Send the ping messages OUTSIDE the lock.
        for (const auto& pubKey : peersToPing)
        {
            MessageP2P pingMsg = MessageManager::createPingMessage(m_myPublicKey);
            signMessage(pingMsg);
            sendMessageTo(pingMsg, pubKey);
            std::cout << "{" << m_myPort << "} " << "[Ping] Sent ping to peer: " << pubKey << std::endl;
        }

        // 3) Sleep until the next check
        std::this_thread::sleep_for(checkInterval);
    }
}

void P2PNode::incrementView()
{
    m_currentView++;
}

void P2PNode::walletProcessNewBlock(const Block& newBlock)
{
    m_myWallet.updateUTXOsFromNewBlock(newBlock._transactions);
}

//void P2PNode::startMining(Block& proposedBlock)
//{
//    // TO-DO: CHECK Might cause errors because working on copy of blockchain
//    const Blockchain* blockchain = getBlockchain();
//    // TO-DO: CHECK Might not change the block because of references. 
//    m_miningThread = std::thread(&Blockchain::mineNewProposedBlock, &blockchain, proposedBlock, getMyPublicKey());
//}
//
//void P2PNode::stopMining()
//{
//    // Indicate mining thread to stop mining
//    m_stopMining.store(true, std::memory_order_release);
//    m_miningThread.join();
//}

void P2PNode::printPeers()
{
    std::lock_guard<std::mutex> lock(m_peerMutex);
    for (auto& kv : m_peers)
    {
        auto peer = kv.second;
        std::cout << peer;
    }
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
