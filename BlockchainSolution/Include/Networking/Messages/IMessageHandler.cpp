#include "pch.h"
#include "IMessageHandler.h"


std::string IMessageHandler::getPublicKey()
{
    return _node->getMyPublicKey();
}


void IMessageHandler::addPeer(PeerInfo info)
{
    _node->addPeer(info);
}
