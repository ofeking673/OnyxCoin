#pragma once


//#include "Server.h"
//#include "Miner.h"
//#include "Client.h"
//
//// Discovery server is localhost:4444
//// Will open this port if we decide to goo full p2p support for nodes outside of subnet
//#define DISCOVERY 4444 
//
//class P2PNode
//{
//public:
//	P2PNode(int handlerIdentifier, std::string keyPath, int port);
//	~P2PNode();
//
//	void pingAll();
//private:
//	std::string signMessage(std::string message);
//
//	IMessageHandler* _handler;
//	MessageManager _messageCreator;
//};



#include "PeerInfo.h"
#include "MessageDispatcher.h"
#include "Wallet.h"
#include "PhaseState.h"

#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <atomic>

#define DISCOVERY_SERVER_PORT 4444
#define LOCALHOST "127.0.0.1"
// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")


class P2PNode
{
public:
    P2PNode(bool isDiscoveryServer = false, const std::string& filePath="");

    ~P2PNode();

    virtual bool start(const std::string& listenAddress, uint16_t port);

    void stop();

    // Connect to a new remote node
    bool connectToNode(const std::string& ip, uint16_t port,
        const std::string& remotePublicKey,
        const std::string& remoteNodeId);
    void connectToPeer(PeerInfo& info);
    // Get peers from discovery server
    void getPeers();
    void sendDiscovery(SOCKET sock);

    // Send a message to a specific peer
    bool sendMessageTo(MessageP2P& msg, const std::string toPublicKey);

    // Broadcast a message to all connected peers
    void broadcastMessage(MessageP2P& msg);

    // Add new peer to the peers map. When establishing a new connection, or when a new node coneects to us.
    void addPeer(const PeerInfo& newPeer);

    // Removes a peer from the peers map
    bool removePeer(const PeerInfo& peer);

    // Update last contact with the peer
    void updatePeersLastContact(const std::string& peerPublicKey);

    // Start a background thread that periodically pings inactive peers
    void startPingThread();

    // Getters
    std::string getMyPublicKey() const;
    uint16_t getMyPort() const;
    PeerInfo getMyInfo() const;

    // Get list of all connected peers (for broadcast, etc)
    std::vector<PeerInfo> getAllClients();

    // Serialize peer list into json
    json peersToJson();
    // Deserialize peer list from json to the peers map
    static std::unordered_map<std::string, PeerInfo> fromJsonToPeers(const json& data);

    // Given two peer maps, return a map of the new peers not found in the owned map.
    std::unordered_map<std::string, PeerInfo> getNewPeers(const std::unordered_map<std::string, PeerInfo>& newPeers);

    // Get the current view number
    uint32_t getCurrentView() const;
    // Set the current view number
    void setCurrentView(uint32_t newView);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------------------------------
    // Phase State Container 
    // ---------------------------------------------------------------------------------------------------------
    // Recieved a pre preapare message. Start tracking it.
    void addNewPhaseState(uint32_t view, int sequence, const Block& block);

    // Check if already got a pre prepare message for that view and seq
    bool isTrackingTheState(uint32_t view, int sequence) const;

    // Add prepare message to the tracker of messages of (view, seq)
    void addPrepareMessage(uint32_t view, int sequence, const MessageP2P& prepareMessage);
    // Add commit message to the tracker of messages of (view, seq)
    void addCommitMessage(uint32_t view, int sequence, const MessageP2P& commitMessage);

    // Get the amount of recieved prepared messages for this (view, sequence)
    int getPrepareAmount(uint32_t view, int sequence);
    // Get the amount of recieved commit messages for this (view, sequence)
    int getCommitAmount(uint32_t view, int sequence);

    // Set the phase(view, seq) as prepared
    void setPrepared(uint32_t view, int sequence);
    // Set the phase(view, seq) as committed
    void setCommitted(uint32_t view, int sequence);

    // Check if the phase(view, seq) is prepared
    bool isPrepared(uint32_t view, int sequence);
    // Check if the phase(view, seq) is committed
    bool isCommitted(uint32_t view, int sequence);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------------------------------
    // VIEW CHANGE Container
    // ---------------------------------------------------------------------------------------------------------
    // Add view change message to the tracker of view change messages
    void addViewChangeMessage(uint32_t newView, MessageP2P viewChangeMessage);

    // Check if already recieved a view change message from this author (for the current view change)
    bool isRecievedViewChangeMessageFromAuthor(uint32_t newView, const std::string& author);

    bool checkRemoteViewChangeMessagesVector(std::vector<MessageP2P> viewChangeMessages);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////


protected:
    // Main loop that accepts incoming connections
    virtual void acceptLoop();

    // Thread routine to receive messages from a single peer
    void receiveLoop(SOCKET sock, const std::string& peerPublicKey);
    
    // Sign a message with my private key
    void signMessage(MessageP2P& msg);

    // Checks peers last contact time, and send ping messages.
    void pingInactivePeers();

    // Increment view number
    void incrementView();

    // Testing
    void printPeers();
protected:
    uint32_t m_currentView;
    std::string m_myNodeId;
    std::string m_myPublicKey;

    SOCKET      m_listenSocket = INVALID_SOCKET;
    std::atomic<bool> m_isRunning;

    // Public key -> peer info
    std::unordered_map<std::string, PeerInfo> m_peers;
    std::mutex  m_peerMutex;

    std::thread m_acceptThread;

    // Flag if this node is a "discovery server" or not
    bool m_isDiscoveryServer;

    // Discovery server's IP/port/public key. 
    std::string m_discoveryServerIp;
    uint16_t    m_discoveryServerPort;
    std::string m_discoveryServerPublicKey;

    // Useful parameters for node
    MessageDispatcher m_dispatcher;
    std::string m_myPrivateKey;
    std::string m_myIP;
    uint16_t m_myPort;

    std::thread m_pingThread;  // Thread that will run ping logic

    // PBFT state
    // Map of (view, sequence) -> phase state of block
    std::map<std::pair<uint32_t, int>, PhaseState> m_phaseStates;

    // Map of(new view) -> VIEW_CHANGE messages
    std::map<uint32_t, std::vector<MessageP2P>> m_viewChangeStates;
};


//// --------------------------------------------------------------------
//// Example Main (Demonstration)
//// --------------------------------------------------------------------
//int main()
//{
//    // Create a node
//    P2PNode myNode("MyNodeId", "MyPublicKey");
//
//    // Start listening on 127.0.0.1:12345
//    if (!myNode.start("127.0.0.1", 12345))
//    {
//        std::cerr << "Failed to start node." << std::endl;
//        return -1;
//    }
//
//    // Suppose we also want to connect to another node listening on 127.0.0.1:12346
//    // We'll pretend we know its publicKey and nodeId
//    myNode.connectToNode("127.0.0.1", 12346, "RemotePublicKey", "RemoteNodeId");
//
//    // Send an example broadcast message
//    myNode.broadcastMessage("PING", "Hello, are you alive?");
//
//    // Keep the node running to accept connections and handle messages
//    std::cout << "[Info] Press ENTER to stop the node." << std::endl;
//    std::cin.get();
//
//    // Stop the node
//    myNode.stop();
//
//    return 0;
//}
