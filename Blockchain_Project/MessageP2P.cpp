#include "MessageP2P.h"

// Constructor
MessageP2P::MessageP2P(const std::string& signature,
    MessageType type,
    uint32_t length,
    const std::vector<uint8_t>& payload)
    : _signature(signature)
    , _type(type)
    , _length(length)
    , _payload(payload)
{
}

// Getters
const std::string& MessageP2P::getSignature() const
{
    return _signature;
}

MessageType MessageP2P::getType() const
{
    return _type;
}

uint32_t MessageP2P::getLength() const
{
    return _length;
}

const std::vector<uint8_t>& MessageP2P::getPayload() const
{
    return _payload;
}

// Setters
void MessageP2P::setSignature(const std::string& signature)
{
    _signature = signature;
}

void MessageP2P::setType(MessageType type)
{
    _type = type;
}

void MessageP2P::setLength(uint32_t length)
{
    _length = length;
}

void MessageP2P::setPayload(const std::vector<uint8_t>& payload)
{
    _payload = payload;
}
