#include "pch.h"
#include "MessageTypes.h"

std::string to_string(MessageType type)
{
    switch (type)
    {
    case MessageType::ERROR_MESSAGE:       return "ERROR_MESSAGE";
    case MessageType::PING:                return "PING";
    case MessageType::PONG:                return "PONG";
    case MessageType::GET_PEERS:           return "GET_PEERS";
    case MessageType::PEER_LIST:           return "PEER_LIST";
    case MessageType::GET_BLOCK:           return "GET_BLOCK";
    case MessageType::BLOCK:               return "BLOCK";
    case MessageType::NEW_TRANSACTION:     return "NEW_TRANSACTION";
    case MessageType::GET_TRANSACTION:     return "GET_TRANSACTION";
    case MessageType::INVENTORY:           return "INVENTORY";
    case MessageType::GET_HEADERS:         return "GET_HEADERS";
    case MessageType::HEADERS:             return "HEADERS";
    case MessageType::HANDSHAKE:           return "HANDSHAKE";
    case MessageType::DISCOVERY_REQUEST:   return "DISCOVERY_REQUEST";
    case MessageType::DISCOVERY_RESPONSE:  return "DISCOVERY_RESPONSE";
    case MessageType::PREPREPARE:          return "PREPREPARE";
    case MessageType::PREPARE:             return "PREPARE";
    case MessageType::COMMIT:              return "COMMIT";
    case MessageType::HASH_READY:          return "HASH_READY";
    case MessageType::VIEW_CHANGE:         return "VIEW_CHANGE";
    case MessageType::NEW_VIEW:            return "NEW_VIEW";
    case MessageType::GET_VIEW:            return "GET_VIEW";
    default:                             return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, MessageType type)
{
    os << to_string(type);
    return os;
}
