#include "IMessageHandler.h"

std::vector<SOCKET> IMessageHandler::getAllClients()
{
    return _peerManager.getAllClients();
}

std::vector<std::string> IMessageHandler::getAllPublicKeys()
{
    return _peerManager.getAllPublicKeys();
}

std::string IMessageHandler::getPublicKey()
{
    return _peerManager.getPubKey();
}

std::string IMessageHandler::signMessage(std::string msg)
{
    return _peerManager.signMessage(msg);
}

std::string IMessageHandler::sendMessage(const std::string& pubkey, MessageP2P msg)
{
    _peerManager.sendMessage(pubkey, msg.toJson().dump());
    try {
        return _peerManager.recvMessage(pubkey);
    }
    catch (std::exception& e) {
        return "";
    }
}

void IMessageHandler::addPeer(const std::string& pubkey, SOCKET sock)
{
    _peerManager.addPeer(pubkey, sock);
}
