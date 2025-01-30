#pragma once

#include <unordered_map>
#include <functional>
#include "MessageP2P.h"
#include "Networking/Socket.h"

class IMessageHandler;

///
/// Dispatches messages to the appropriate handler function based on the message type.
///
class MessageDispatcher
{
public:
    /// Constructor takes an IMessageHandler interface implementation.
    explicit MessageDispatcher(IMessageHandler* handler);

    /// Dispatch the message by calling the correct handler method.
    std::vector<MessageP2P> dispatch(const MessageP2P& msg);

    ///// Send a message to the specified user, return the answer if there is one, else, return ""
    //std::string sendClient(const std::string& pubkey, const MessageP2P& msg);

    //bool isKnownUser(const std::string& pubkey);
    //void addPeer(const std::string& pubkey, SOCKET sock);
private:
    IMessageHandler* _handler;
    static std::unordered_map<MessageType, std::function<std::vector<MessageP2P>(IMessageHandler*, const MessageP2P&)>> _dispatchTable;

};
