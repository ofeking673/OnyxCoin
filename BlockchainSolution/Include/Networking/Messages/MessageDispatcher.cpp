#include "pch.h"
#include "MessageDispatcher.h"
#include "FullNodeMessageHandler.h"

std::unordered_map<MessageType, std::function<std::vector<MessageP2P>(IMessageHandler*, const MessageP2P&)>> MessageDispatcher::_dispatchTable;

// Constructor: store the handler pointer and initialize a dispatch table.
MessageDispatcher::MessageDispatcher(P2PNode* node, bool isGenesis)
    : _handler(new FullNodeMessageHandler(node, isGenesis))
{
    // Build a static dispatch table once, so we don't reconstruct it on every dispatch.
    _dispatchTable =
    {
        {
            MessageType::ERROR_MESSAGE,
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onError(msg); }
        },
        {
            MessageType::PING,
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onPing(msg); }
        },                                                   
        {                                                    
            MessageType::PONG,                               
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onPong(msg); }
        },                                                   
        {                                                    
            MessageType::GET_PEERS,                          
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onGetPeers(msg); }
        },                                                   
        {                                                    
            MessageType::PEER_LIST,                          
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onPeerList(msg); }
        },                                                   
        {                                                    
            MessageType::GET_BLOCK,                          
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onGetBlock(msg); }
        },                                                   
        {                                                    
            MessageType::BLOCK,                              
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onBlock(msg); }
        },                                                   
        {                                                    
            MessageType::NEW_TRANSACTION,                    
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onNewTransaction(msg); }
        },                                                   
        {                                                    
            MessageType::GET_TRANSACTION,                    
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onGetTransaction(msg); }
        },                                                   
        {                                                    
            MessageType::INVENTORY,                          
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onInventory(msg); }
        },                                                   
        {                                                    
            MessageType::GET_HEADERS,                        
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onGetHeaders(msg); }
        },                                                   
        {                                                    
            MessageType::HEADERS,                            
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onHeaders(msg); }
        },
        {
            MessageType::HANDSHAKE,
            [](IMessageHandler* h, const MessageP2P& msg) { return h->onHandshake(msg); }
        }
    };
}

// Dispatch the message by calling the correct handler method.
std::vector<MessageP2P> MessageDispatcher::dispatch(const MessageP2P& msg)
{
    if (!_handler) 
    {
        // If there's no handler set, just return
        return {};
    }
    
    // Find the message type in the dispatch table
    auto it = _dispatchTable.find(msg.getType());
    if (it != _dispatchTable.end())
    {
        // Call the associated handler
        return (it->second)(_handler, msg);
    }
    else
    {
        // Handle unknown or unimplemented message type
        std::cout << "Unknown message type received" << std::endl;
        return {};
    }
}

Blockchain* MessageDispatcher::getChain() const
{
    return _handler->_blockchain;
}

void MessageDispatcher::setBlockchainSingleton()
{
    _handler->_blockchain->setSingleton();
}

//std::string MessageDispatcher::sendClient(const std::string& pubkey, const MessageP2P& msg)
//{
//    if (isKnownUser(pubkey)) {
//        return _handler->sendMessage(pubkey, msg);
//    }
//    return "";
//}
//
//bool MessageDispatcher::isKnownUser(const std::string& pubkey)
//{
//    auto clients = _handler->getAllPublicKeys();
//    return std::find(clients.begin(), clients.end(), pubkey) != clients.end();
//}
//
//void MessageDispatcher::addPeer(const std::string& pubkey, SOCKET sock)
//{
//    _handler->addPeer(pubkey, sock);
//}
