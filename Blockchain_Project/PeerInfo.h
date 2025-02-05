#pragma once

#include <winsock2.h>
#include <string>
#include <chrono>
#include "json.hpp"

using json = nlohmann::json;

class PeerInfo
{

public:
    PeerInfo(const std::string& ip, const uint16_t& port, const std::string& publicKey, const std::string& nodeId);
    PeerInfo() = default;

    json toJson() const;
    static PeerInfo fromJson(const json& data);

    std::string ip;         // IP of peer
    uint16_t port;          // Port to establish communication with peer
    std::string publicKey;  // Public key of the peer
    std::string nodeId;     // Global node ID (e.g. can be a unique string or hashed public key)
    SOCKET      socket;     // The socket to communicate with this peer
    std::chrono::steady_clock::time_point lastContact; // When we last heard from this peer

    // Print a PeerInfo object.
    friend std::ostream& operator<<(std::ostream& os, const PeerInfo& peer);
};

