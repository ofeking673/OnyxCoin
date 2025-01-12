#include "MessageManager.h"

MessageP2P MessageManager::createPingMessage(const std::string& privateKey)
{
    MessageP2P message;
    message.setType(MessageType::PING);
    
    signMessage(message, privateKey);

    return message;
}

MessageP2P MessageManager::createPongMessage(const std::string& privateKey)
{
    MessageP2P message;
    message.setType(MessageType::PONG);
    
    signMessage(message, privateKey);

    return message;
}

MessageP2P MessageManager::createGetPeersMessage(const std::string& privateKey)
{
    MessageP2P message;
    message.setType(MessageType::GET_PEERS);

    signMessage(message, privateKey);

    return message;
}
