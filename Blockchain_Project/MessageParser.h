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

    // Parses GET_HEADERS message
    // Returns vector of pairs of block hash and previous block hash,
    // and the stop hash
    static const void parseGetHeadersMessage(const MessageP2P& msg, std::vector<std::pair<std::string, std::string>>& blockHashes, std::string& stopHash);
private:
    // Private constructor to prevent instantiation; all methods are static.
    MessageParser() = default;
};
