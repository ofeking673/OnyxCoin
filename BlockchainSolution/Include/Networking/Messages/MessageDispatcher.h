#pragma once

#include <unordered_map>
#include <functional>
#include "MessageP2P.h"

class IMessageHandler;
class FullNodeMessageHandler;
class P2PNode;

///
/// Dispatches messages to the appropriate handler function based on the message type.
///
class MessageDispatcher
{
public:
    /// Constructor takes an IMessageHandler interface implementation.
    MessageDispatcher(P2PNode* node, bool isGenesis);

    /// Dispatch the message by calling the correct handler method.
    std::vector<MessageP2P> dispatch(const MessageP2P& msg);

    Blockchain* getChain() const;
private:
    FullNodeMessageHandler* _handler;
    static std::unordered_map<MessageType, std::function<std::vector<MessageP2P>(IMessageHandler*, const MessageP2P&)>> _dispatchTable;

};
