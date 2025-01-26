#include "P2PNode.h"
#include "FullNodeMessageHandler.h"

P2PNode::P2PNode(int handlerIdentifier, std::string keyPath, int port)
{
	switch (handlerIdentifier)
	{
		case(FULL_NODE_MESSAGE_HANDLER):
			_handler = new FullNodeMessageHandler(keyPath, port);
			break;
		default:
			_handler = nullptr;
	}
}

P2PNode::~P2PNode()
{
	delete _handler;
}

void P2PNode::pingAll()
{
	MessageP2P pingMsg = _messageCreator.createPingMessage(_handler->getPublicKey());
	std::string signature = signMessage(pingMsg.toJson().dump());
	pingMsg.setSignature(signature);
	std::vector<SOCKET> clients = _handler->getAllClients();
	for (const auto& client : clients) {
		std::string msg = pingMsg.toJson().dump();
		Socket::sendMessage(client, msg);
	}
}

std::string P2PNode::signMessage(std::string message)
{
	return _handler->signMessage(message);
}
