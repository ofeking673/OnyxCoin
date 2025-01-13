#include "MessageManager.h"

MessageP2P MessageManager::createPingMessage(const std::string& publicKey)
{
    MessageP2P message;
    
    message.setType(MessageType::PING);
    message.setAuthor(publicKey);


    return message;
}

MessageP2P MessageManager::createPongMessage(const std::string& publicKey)
{
    MessageP2P message;
    message.setType(MessageType::PONG);
    message.setAuthor(publicKey);


    return message;
}


MessageP2P MessageManager::createGetPeersMessage(const std::string& publicKey)
{
    MessageP2P message;
    message.setType(MessageType::GET_PEERS);
    message.setAuthor(publicKey);


    return message;
}
