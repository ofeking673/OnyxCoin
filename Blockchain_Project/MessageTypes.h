#pragma once

#include <cstdint>

/// Define all supported message types.
enum class MessageType : uint16_t
{
    PING = 1,
    PONG,
    GET_PEERS,
    PEER_LIST,
    GET_BLOCK,
    BLOCK,
    NEW_TRANSACTION,
    TRANSACTION,
    INVENTORY,
    GET_HEADERS,
    HEADERS
};
