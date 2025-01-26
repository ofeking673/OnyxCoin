#pragma once

#include <string>
#include <vector>
#include "MessageTypes.h"
#include "json.hpp"


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
        nlohmann::json payload);

    /// Getters
    const std::string& getSignature() const;
    const std::string& getAuthor() const;
    MessageType        getType() const;
    uint32_t           getLength() const;
    const nlohmann::json getPayload() const;

    /// Setters
    void setSignature(const std::string& signature);
    void setAuthor(const std::string& author);
    void setType(MessageType type);
    void setLength(uint32_t length);
    void setPayload(nlohmann::json payload);
    /// Utility
    nlohmann::json toJson() const;
    static MessageP2P fromJson(nlohmann::json data);
private:
    // Header fields
    std::string _signature;
    std::string _author;
    MessageType _type;
    uint32_t    _payloadLength;

    // Actual message data
    nlohmann::json _payload;
};
