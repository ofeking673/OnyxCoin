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
