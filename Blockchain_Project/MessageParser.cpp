#include "MessageParser.h"
#include "MessageP2P.h"
#include <cstdint>
#include "json.hpp"

using nlohmann::json;

MessageP2P MessageParser::parse(const std::string& buffer)
{
    MessageP2P msg = MessageP2P::fromJson(buffer);
    return msg;

    //// Minimum bytes needed
    //static constexpr size_t M_HEADER_SIZE = M_SIGNATURE_SIZE + M_AUTHOR_SIZE + M_TYPE_SIZE + M_PAYLOAD_LENGTH_SIZE;

    //if (buffer.size() < M_HEADER_SIZE)
    //{
    //    return MessageP2P(); // Not enough data to parse the header
    //}

    //// Parse signature
    //std::string signature;
    //if(!parseSignature(buffer, signature))
    //{
    //    // Couldn't parse signature
    //    return MessageP2P();
    //}

    //// Parse author
    //std::string author;
    //if(!parseAuthor(buffer, author))
    //{
    //    // Couldn't parse author
    //    return MessageP2P();
    //}

    //// Parse message type
    //MessageType messageType;
    //if (!parseMessageType(buffer, messageType))
    //{
    //    // Couldn't parse type
    //    return MessageP2P();
    //}

    //// Parse payload length
    //uint32_t payloadLength;
    //if(!parsePayloadLength(buffer, payloadLength))
    //{
    //    // Couldn't parse payload length
    //    return MessageP2P();
    //}

    //// Parse payload
    //std::string payload;
    //if (!parsePayload(buffer, payload, payloadLength))
    //{
    //    // Couldn't parse payload
    //    return MessageP2P();
    //}


    //// Construct the MessageP2P object
    //MessageP2P msg = MessageP2P(
    //    signature,
    //    author,
    //    messageType,
    //    payloadLength,
    //    payload
    //);

    //return msg;
}

const std::string MessageParser::parseGetTransactionMessage(const MessageP2P& msg)
{
    // Parse the JSON string back into a JSON object
    json j = msg.getPayload();

    // Retrieve the value for "txID"
    std::string parsedTxID = j["txID"].get<std::string>();

    return parsedTxID;
}

const std::pair<std::string, std::string> MessageParser::parseGetBlockMessage(const MessageP2P& msg)
{
    // Parse the JSON string back into a JSON object
    json j = msg.getPayload();

    // Retrieve the values for block hash and previous block hash
    std::string blockHash = j["blockHash"].get<std::string>();
    std::string prevBlockHash = j["prevBlockHash"].get<std::string>();


    std::pair<std::string, std::string> blkHash;
    blkHash.first = blockHash;
    blkHash.second = prevBlockHash;

    return blkHash;
}

const void MessageParser::parseGetHeadersMessage(const MessageP2P& msg, std::vector<std::pair<std::string, std::string>>& blockHashes, std::string& stopHash)
{
    // Parse the JSON string into a json object.
    json j = msg.getPayload();


    // Clear blockHashes and stopHash in case they have previous data.
    blockHashes.clear();
    stopHash.clear();

    // Check if the expected keys exist.
    if (!j.contains("blockHashes") || !j.contains("stopHash")) 
    {
        return;
    }

    // Parse stopHash.
    stopHash = j["stopHash"].get<std::string>();

    // Parse the blockHashes array.
    // Each element is expected to be an object containing "blockHash" and "prevBlockHash"
    for (const auto& element : j["blockHashes"]) 
    {
        std::string blockHash = element.at("blockHash").get<std::string>();
        std::string prevBlockHash = element.at("prevBlockHash").get<std::string>();
        blockHashes.emplace_back(blockHash, prevBlockHash);
    }
}

bool MessageParser::parseSignature(const std::string& buffer, std::string& signature)
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


bool MessageParser::parseAuthor(const std::string& buffer, std::string& author)
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

bool MessageParser::parseMessageType(const std::string& buffer, MessageType& messageType)
{
    size_t startPosMessageType = M_SIGNATURE_SIZE + M_AUTHOR_SIZE;

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

bool MessageParser::parsePayloadLength(const std::string& buffer, uint32_t& payloadLength)
{
    size_t startPosPayloadLength = M_SIGNATURE_SIZE + M_AUTHOR_SIZE + M_TYPE_SIZE;

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

bool MessageParser::parsePayload(const std::string& buffer, std::string& payload, const uint32_t& payloadLength)
{
    static constexpr size_t M_HEADER_SIZE = M_SIGNATURE_SIZE + M_AUTHOR_SIZE + M_TYPE_SIZE + M_PAYLOAD_LENGTH_SIZE;

    // TO-DO: add better checks for parsing


    // Check if we have enough bytes for the payload
    if (buffer.size() < M_HEADER_SIZE + payloadLength)
    {
        return false;
    }

    // Extract the payload
    std::copy(buffer.begin() + M_HEADER_SIZE,
        buffer.begin() + M_HEADER_SIZE + payloadLength,
        payload.begin());

    return true;

}

