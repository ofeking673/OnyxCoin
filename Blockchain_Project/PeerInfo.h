#pragma once

#include <winsock2.h>
#include <string>
#include <chrono>

class PeerInfo
{

public:
    std::string publicKey;  // Public key of the peer
    std::string nodeId;     // Global node ID (e.g. can be a unique string or hashed public key)
    SOCKET      socket;     // The socket to communicate with this peer
    std::chrono::steady_clock::time_point lastContact; // When we last heard from this peer
};

