#pragma once

#include <cstdint>
#include <string>
#include <iostream>

#define DISCOVERY_PAYLOAD_LISTEN_IP "LISTEN_IP"
#define DISCOVERY_PAYLOAD_LISTEN_PORT "LISTEN_PORT"
#define DISCOVERY_PAYLOAD_ASSIGNED_ID "ID"
#define DISCOVERY_PAYLOAD_NODE_LIST "LIST"


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
    HEADERS,
    HANDSHAKE,
    DISCOVERY_REQUEST,
    DISCOVERY_RESPONSE,
    PREPREPARE,
    PREPARE,
    COMMIT,
    HASH_READY,
    VIEW_CHANGE,
    NEW_VIEW,
    GET_VIEW,
};


// Function prototype to convert a MessageType to its string representation.
std::string to_string(MessageType type);

// Overload the stream insertion operator for MessageType.
std::ostream& operator<<(std::ostream& os, MessageType type);