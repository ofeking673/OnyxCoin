#pragma once

#include <cstdint>

/// Define all supported message types.
enum class MessageType : uint16_t
{
    ERROR_MESSAGE,
    PING,
    PONG,
    GET_PEERS,
    PEER_LIST,
    GET_BLOCK,
    BLOCK,
    NEW_TRANSACTION,
    GET_TRANSACTION,
    INVENTORY,
    GET_HEADERS,
    HEADERS
};
