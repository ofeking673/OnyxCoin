#include "P2PNode.h"

void P2PNode::pingAll()
{
	MessageP2P pingMsg = messageCreator.createPingMessage(handler.getPublicKey());
	pingMsg.setSignature(signMessage(pingMsg.ToString()));
	std::vector<SOCKET> clients = handler.getAllClients();
	for (const auto& client : clients) {
		std::string msg = pingMsg.ToString();
		Socket::sendMessage(client, msg);
	}
}

std::string P2PNode::signMessage(std::string message)
{
	return handler.signMessage(message);
}
