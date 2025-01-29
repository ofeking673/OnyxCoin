#pragma once
#include <string>

class NetworkNodeInfo
{
public:
    std::string ip;
    uint16_t    port;
    std::string publicKey;
    std::string nodeId;  // The "unique" ID assigned by discovery server
};

