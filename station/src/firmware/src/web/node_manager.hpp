#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <network/websocket/messages/client_messages.hpp>

namespace SyncBlink
{
    class NodeManager
    {
    public:
        Client::CountedMessage counted = { LED_COUNT, 1, LED_COUNT, 1 };
    };
}

#endif // NODEMANAGER_H