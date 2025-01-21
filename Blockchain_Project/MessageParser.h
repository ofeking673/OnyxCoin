#pragma once

#include <memory>
#include <vector>
#include "MessageP2P.h"

#define M_SIGNATURE_SIZE      16
#define M_AUTHOR_SIZE         40
#define M_TYPE_SIZE           2
#define M_PAYLOAD_LENGTH_SIZE 4

// Responsible for parsing raw byte streams into MessageP2P objects.
class MessageParser
{
public:
    /// Parses a raw buffer and returns a newly created MessageP2P object.
    /// Returns MessageP2P with ERROR type, if the buffer is invalid or incomplete.
    static MessageP2P parse(const std::string& buffer);

    // Parses GET_TRANSACTION message
    // Returns transaction ID
    static const std::string parseGetTransactionMessage(const MessageP2P& msg);

    // Parses GET_BLOCK message
    // Returns pair of block hash and previous block hash
    static const std::pair<std::string, std::string> parseGetBlockMessage(const MessageP2P& msg);

private:
    // Private constructor to prevent instantiation; all methods are static.
    MessageParser() = default;

    // Optional helper methods for parsing could be declared here, e.g.:
    static bool parseSignature(const std::string& buffer, std::string& signature);
    static bool parseAuthor(const std::string& buffer, std::string& author);
    static bool parseMessageType(const std::string& buffer, MessageType& messageType);
    static bool parsePayloadLength(const std::string& buffer, uint32_t& payloadLength);
    static bool parsePayload(const std::string& buffer, std::string& payload, const uint32_t& payloadLength);
};
