#pragma once

#include <unordered_map>
#include <functional>
#include "IMessageHandler.h"
#include "MessageTypes.h"

///
/// Dispatches messages to the appropriate handler function based on the message type.
///
class MessageDispatcher
{
public:
    /// Constructor takes an IMessageHandler interface implementation.
    explicit MessageDispatcher(IMessageHandler* handler);

    /// Dispatch the message by calling the correct handler method.
    void dispatch(const MessageP2P& msg);

private:
    IMessageHandler* _handler;
};
