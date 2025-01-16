#include "MessageDispatcher.h"
#include "MessageP2P.h"
#include <unordered_map>
#include <functional>
#include <iostream>

std::unordered_map<MessageType, std::function<void(IMessageHandler*, const MessageP2P&)>> MessageDispatcher::_dispatchTable;

// Constructor: store the handler pointer and initialize a dispatch table.
MessageDispatcher::MessageDispatcher(IMessageHandler* handler)
    : _handler(handler)
{
    // Build a static dispatch table once, so we don't reconstruct it on every dispatch.
    _dispatchTable =
    {
        {
            MessageType::ERROR,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onError(msg); }
        },
        {
            MessageType::PING,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onPing(msg); }
        },
        {
            MessageType::PONG,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onPong(msg); }
        },
        {
            MessageType::GET_PEERS,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onGetPeers(msg); }
        },
        {
            MessageType::PEER_LIST,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onPeerList(msg); }
        },
        {
            MessageType::GET_BLOCK,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onGetBlock(msg); }
        },
        {
            MessageType::BLOCK,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onBlock(msg); }
        },
        {
            MessageType::NEW_TRANSACTION,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onNewTransaction(msg); }
        },
        {
            MessageType::GET_TRANSACTION,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onGetTransaction(msg); }
        },
        {
            MessageType::INVENTORY,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onInventory(msg); }
        },
        {
            MessageType::GET_HEADERS,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onGetHeaders(msg); }
        },
        {
            MessageType::HEADERS,
            [](IMessageHandler* h, const MessageP2P& msg) { h->onHeaders(msg); }
        }
    };
}

// Dispatch the message by calling the correct handler method.
void MessageDispatcher::dispatch(const MessageP2P& msg)
{
    if (!_handler) 
    {
        // If there's no handler set, just return
        return;
    }
    
    // Find the message type in the dispatch table
    auto it = _dispatchTable.find(msg.getType());
    if (it != _dispatchTable.end())
    {
        // Call the associated handler
        (it->second)(_handler, msg);
    }
    else
    {
        // Handle unknown or unimplemented message type
        std::cout << "Unknown message type received" << std::endl;
    }
}
