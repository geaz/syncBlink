#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "deps/websockets/src/WebSocketsClient.h"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <functional>

namespace SyncBlink
{
    typedef std::function<void(std::string mod)> MeshModEvent;
    typedef std::function<void(Server::Message message)> ClientMessageEvent;

    class SocketClient
    {
    public:
        void start(String socketIp);
        void loop();
        void sendMessage(Client::Message message);

        bool isConnected() const;

        EventRegistration<MeshModEvent> meshModEvents;
        EventRegistration<ClientMessageEvent> messageEvents;

    private:
        void clientEvent(WStype_t type, uint8_t *payload, size_t length);

        WebSocketsClient _webSocket;
        bool _isConnected = false;
    };
}

#endif // SOCKETCLIENT_H