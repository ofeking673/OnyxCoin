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
        MessageType type,
        uint32_t length,
        const std::vector<uint8_t>& payload);

    /// Getters
    const std::string& getSignature() const;
    MessageType        getType() const;
    uint32_t           getLength() const;
    const std::vector<uint8_t>& getPayload() const;

    /// Setters
    void setSignature(const std::string& signature);
    void setType(MessageType type);
    void setLength(uint32_t length);
    void setPayload(const std::vector<uint8_t>& payload);

private:
    // Header fields
    std::string _signature;
    MessageType _type;
    uint32_t    _payloadLength;

    // Actual message data
    std::vector<uint8_t> _payload;
};
