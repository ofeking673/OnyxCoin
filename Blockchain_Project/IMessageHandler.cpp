#include "IMessageHandler.h"

std::vector<SOCKET> IMessageHandler::getAllClients()
{
    return _peerManager.getAllClients();
}

std::string IMessageHandler::getPublicKey()
{
    return _peerManager.getPubKey();
}

std::string IMessageHandler::signMessage(std::string msg)
{
    return _peerManager.signMessage(msg);
}
