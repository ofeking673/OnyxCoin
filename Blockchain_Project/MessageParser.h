#pragma once

#include <memory>
#include <vector>
#include "MessageP2P.h"

#define M_SIGNATURE_SIZE      16
#define M_AUTHOR_SIZE         128
#define M_TYPE_SIZE           2
#define M_PAYLOAD_LENGTH_SIZE 4

// Responsible for parsing raw byte streams into MessageP2P objects.
class MessageParser
{
public:
    /// Parses a raw buffer and returns a newly created MessageP2P object.
    /// Returns nullptr if the buffer is invalid or incomplete.
    static std::unique_ptr<MessageP2P> parse(const std::vector<uint8_t>& buffer);

private:
    // Private constructor to prevent instantiation; all methods are static.
    MessageParser() = default;

    // Optional helper methods for parsing could be declared here, e.g.:
    static bool parseSignature(const std::vector<uint8_t>& buffer, std::string& signature);
    static bool parseAuthor(const std::vector<uint8_t>& buffer, std::string& author);
    static bool parseMessageType(const std::vector<uint8_t>& buffer, MessageType& messageType);
    static bool parsePayloadLength(const std::vector<uint8_t>& buffer, uint32_t& payloadLength);
    static bool parsePayload(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& payload, const uint32_t& payloadLength);
};
