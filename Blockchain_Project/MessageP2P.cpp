#include "MessageP2P.h"

using json = nlohmann::json;

// Constructor

MessageP2P::MessageP2P()
{
    _signature = "";
    _author = "";
    _type = MessageType::ERROR_MESSAGE;
    _payloadLength = 0;
    json j;
    _payload = j;
}

MessageP2P::MessageP2P(const std::string& signature,
    const std::string& author,
    MessageType type,
    uint32_t length,
    json payload)
    : _signature(signature)
    , _author(author)
    , _type(type)
    , _payloadLength(length)
    , _payload(payload)
{
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

const json MessageP2P::getPayload() const
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

void MessageP2P::setPayload(json payload)
{
    _payload = payload;
}

// Utility: Serialize the message to a JSON string.
json MessageP2P::toJson() const
{
    json j;
    j["signature"] = _signature;
    j["author"] = _author;
    // Store enum as an integer (uint16_t)
    j["type"] = static_cast<uint16_t>(_type);
    j["length"] = _payloadLength;
    j["payload"] = _payload;

    return j;
}

// Utility: Deserialize a JSON string to a MessageP2P instance.
MessageP2P MessageP2P::fromJson(json data)
{
    json j = data;

    std::string signature = j["signature"].get<std::string>();
    std::string author = j["author"].get<std::string>();

    // Convert the numeric JSON value to the MessageType enum
    MessageType type = static_cast<MessageType>(j["type"].get<uint16_t>());

    uint32_t length = j["length"].get<uint32_t>();

    return MessageP2P(signature, author, type, length, j["payload"]);
}
