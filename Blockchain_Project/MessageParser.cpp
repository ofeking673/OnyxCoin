#include "MessageParser.h"
#include "MessageP2P.h"
#include <cstdint>

std::unique_ptr<MessageP2P> MessageParser::parse(const std::vector<uint8_t>& buffer)
{
    // Minimum bytes needed
    static constexpr size_t M_HEADER_SIZE = M_SIGNATURE_SIZE + M_TYPE_SIZE + M_PAYLOAD_LENGTH_SIZE;

    if (buffer.size() < M_HEADER_SIZE)
    {
        return nullptr; // Not enough data to parse the header
    }

    // Parse signature
    std::string signature;
    if(!parseSignature(buffer, signature))
    {
        // Couldn't parse signature
        return nullptr;
    }

    // Parse message type
    MessageType messageType;
    if (!parseMessageType(buffer, messageType))
    {
        // Couldn't parse type
        return nullptr;
    }

    // Parse payload length
    uint32_t payloadLength;
    if(!parsePayloadLength(buffer, payloadLength))
    {
        // Couldn't parse payload length
        return nullptr;
    }

    // Parse payload
    std::vector<uint8_t> payload;
    if (!parsePayload(buffer, payload, payloadLength))
    {
        // Couldn't parse payload
        return nullptr;
    }


    // Construct the MessageP2P object
    std::unique_ptr<MessageP2P> msg = std::make_unique<MessageP2P>(
        signature,
        messageType,
        payloadLength,
        payload
    );

    return msg;
}

bool MessageParser::parseSignature(const std::vector<uint8_t>& buffer, std::string& signature)
{
    signature.reserve(M_SIGNATURE_SIZE);

    // TO-DO: add better checks for parsing


    // Check if buffer contains signature size
    if (buffer.size() < M_SIGNATURE_SIZE)
    {
        return false;
    }

    // Parse the signature as a string
    for (size_t i = 0; i < M_SIGNATURE_SIZE; i++)
    {
        signature.append(1, static_cast<char>(buffer[i]));
    }
    return true;
}

bool MessageParser::parseAuthor(const std::vector<uint8_t>& buffer, std::string& author)
{
    author.reserve(M_AUTHOR_SIZE);

    // TO-DO: add better checks for parsing


    // Check if buffer contains signature size
    if (buffer.size() < M_SIGNATURE_SIZE + M_AUTHOR_SIZE)
    {
        return false;
    }

    // Parse the signature as a string
    for (size_t i = M_SIGNATURE_SIZE; i < M_AUTHOR_SIZE + M_SIGNATURE_SIZE; i++)
    {
        author.append(1, static_cast<char>(buffer[i]));
    }
    return true;
}

bool MessageParser::parseMessageType(const std::vector<uint8_t>& buffer, MessageType& messageType)
{
    size_t startPosMessageType = M_SIGNATURE_SIZE;

    // TO-DO: add better checks for parsing


    // Check if buffer contains type size
    if (buffer.size() < startPosMessageType + M_TYPE_SIZE)
    {
        return false;
    }

    // Parse the 2-byte message type (big-endian)
    uint16_t rawType = (static_cast<uint16_t>(buffer[startPosMessageType]) << 8)
        | static_cast<uint16_t>(buffer[startPosMessageType + 1]);

    messageType = static_cast<MessageType>(rawType);
    return true;
}

bool MessageParser::parsePayloadLength(const std::vector<uint8_t>& buffer, uint32_t& payloadLength)
{
    size_t startPosPayloadLength = M_SIGNATURE_SIZE + M_TYPE_SIZE;

    // TO-DO: add better checks for parsing


    // Check if buffer contains type size
    if (buffer.size() < startPosPayloadLength + M_PAYLOAD_LENGTH_SIZE)
    {
        return false;
    }

    // Parse the 4-byte payload length (big-endian)
    uint32_t rawLength = (static_cast<uint32_t>(buffer[startPosPayloadLength]) << 24)
        | (static_cast<uint32_t>(buffer[startPosPayloadLength + 1]) << 16)
        | (static_cast<uint32_t>(buffer[startPosPayloadLength + 2]) << 8)
        |  static_cast<uint32_t>(buffer[startPosPayloadLength + 3]);

    return true;
}

bool MessageParser::parsePayload(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& payload, const uint32_t& payloadLength)
{
    static constexpr size_t M_HEADER_SIZE = M_SIGNATURE_SIZE + M_TYPE_SIZE + M_PAYLOAD_LENGTH_SIZE;

    // TO-DO: add better checks for parsing


    // Check if we have enough bytes for the payload
    if (buffer.size() < M_HEADER_SIZE + payloadLength)
    {
        return false;
    }

    // Extract the payload
    std::vector<uint8_t> payloadExtract(payloadLength);
    std::copy(buffer.begin() + M_HEADER_SIZE,
        buffer.begin() + M_HEADER_SIZE + payloadLength,
        payload.begin());

    payload = payloadExtract;
    return true;

}

