#include "MessageP2P.h"

// Constructor

MessageP2P::MessageP2P()
{
    _signature = "";
    _type = MessageType::PING;
    _payloadLength = 0;
    _payload.clear();
}

MessageP2P::MessageP2P(const std::string& signature,
    std::string author,
    MessageType type,
    uint32_t length,
    const std::string& payload)
    : _signature(signature)
    , _author(author)
    , _type(type)
    , _payloadLength(length)
    , _payload(payload)
{
}

void MessageP2P::parseJson(json j)
{
    _signature = j["signature"];
    _author = j["author"];
    _type = j["type"];
    _payloadLength = j["payloadLength"];
    _payload = j["payload"];
}

// Getters
const std::string& MessageP2P::getSignature() const
{
    return _signature;
}

const std::string& MessageP2P::getAuthor() const
{
    return _author;
}

MessageType MessageP2P::getType() const
{
    return _type;
}

uint32_t MessageP2P::getLength() const
{
    return _payloadLength;
}

const std::string& MessageP2P::getPayload() const
{
    return _payload;
}

// Setters
void MessageP2P::setSignature(const std::string& signature)
{
    _signature = signature;
}

void MessageP2P::setAuthor(const std::string& author)
{
    _author = author;
}

void MessageP2P::setType(MessageType type)
{
    _type = type;
}

void MessageP2P::setLength(uint32_t length)
{
    _payloadLength = length;
}

void MessageP2P::setPayload(const std::string& payload)
{
    _payload = payload;
}

std::string MessageP2P::ToString() const
{
    json j;
    j["signature"] = _signature;
    j["author"] = _author;
    j["type"] = _type;
    j["payloadLength"] = _payloadLength;
    j["payload"] = _payload;

    return j.dump();
}
