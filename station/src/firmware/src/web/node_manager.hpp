#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <network/websocket/messages/client_messages.hpp>

namespace SyncBlink
{
    class NodeManager
    {
    public:
        Client::CountedMessage counted = {0,0,0,0};
    };
}

#endif // NODEMANAGER_H