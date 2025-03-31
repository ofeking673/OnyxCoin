#pragma once

#include <string>
#include <vector>
#include "MessageTypes.h"
#include "Networking/json.hpp"

using json = nlohmann::json;

// Represents a P2P message with a header and payload.
class MessageP2P
{
public:
    /// Constructors
    MessageP2P();
    MessageP2P(const std::string& signature, 
        const std::string& author,
        MessageType type,
        uint32_t length,
        json payload);

    MessageP2P(const MessageP2P& other);

    /// Getters
    const std::string& getSignature() const;
    const std::string& getAuthor() const;
    MessageType        getType() const;
    uint32_t           getLength() const;
    const json getPayload() const;

    /// Setters
    void setSignature(const std::string& signature);
    void setAuthor(const std::string& author);
    void setType(MessageType type);
    void setLength(uint32_t length);
    void setPayload(json payload);
    /// Utility
    json toJson() const;
    static MessageP2P fromJson(json data);
private:
    // Header fields
    std::string _signature;
    std::string _author;
    MessageType _type;
    uint32_t    _payloadLength;

    // Actual message data
    json _payload;
};
