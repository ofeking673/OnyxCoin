#pragma once

#include <string>
#include <vector>
#include "MessageTypes.h"


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
        const std::string& payload);

    /// Getters
    const std::string& getSignature() const;
    const std::string& getAuthor() const;
    MessageType        getType() const;
    uint32_t           getLength() const;
    const std::string& getPayload() const;

    /// Setters
    void setSignature(const std::string& signature);
    void setAuthor(const std::string& author);
    void setType(MessageType type);
    void setLength(uint32_t length);
    void setPayload(const std::string& payload);
    void parsePayload(const std::string& payload);
    /// Utility
    std::string toJson() const;
    static MessageP2P fromJson(const std::string& data);
private:
    // Header fields
    std::string _signature;
    std::string _author;
    MessageType _type;
    uint32_t    _payloadLength;

    // Actual message data
    std::string _payload;
};
