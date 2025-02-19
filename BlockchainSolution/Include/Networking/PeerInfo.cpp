#include "pch.h"
#include "PeerInfo.h"

PeerInfo::PeerInfo(const std::string& ip, const uint16_t& port, const std::string& publicKey, const std::string& nodeId)
    : ip(ip)
    , port(port)
    , publicKey(publicKey)
    , nodeId(nodeId)
{
}

json PeerInfo::toJson() const
{
    json j;

    // Basic fields
    j["ip"] = ip;
    j["port"] = port;
    j["publicKey"] = publicKey;
    j["nodeId"] = nodeId;

    return j;
}

PeerInfo PeerInfo::fromJson(const json& data)
{
    PeerInfo p;

    // Safely extract fields
    p.ip = data.value("ip", "");
    p.port = data.value("port", 0);
    p.publicKey = data.value("publicKey", "");
    p.nodeId = data.value("nodeId", "");

    return p;
}

std::ostream& operator<<(std::ostream& os, const PeerInfo& peer)
{
    // Convert lastContact to seconds since the steady clock's epoch for readability.
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(
        peer.lastContact.time_since_epoch()).count();

    os << "PeerInfo {"
        << "\n  IP: " << peer.ip
        << "\n  Port: " << peer.port
        << "\n  Public Key: " << peer.publicKey
        << "\n  Node ID: " << peer.nodeId
        << "\n  Socket: " << peer.socket
        << "\n  Last Contact: " << secs << " seconds since steady_clock epoch"
        << "\n}";
    return os;
}
