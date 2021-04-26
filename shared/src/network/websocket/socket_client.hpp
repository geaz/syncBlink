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
    typedef std::function<void(bool isConnected)> ClientConnectionEvent;

    class SocketClient
    {
    public:
        void start(String socketIp, float firmwareVersion);
        void loop();
        void sendMessage(Client::Message message);

        bool isConnected() const;

        EventRegistration<MeshModEvent> meshModEvents;
        EventRegistration<ClientMessageEvent> messageEvents;
        EventRegistration<ClientConnectionEvent> connectionEvents;

    private:
        void clientEvent(WStype_t type, uint8_t *payload, size_t length);

        WebSocketsClient _webSocket;
        bool _isConnected = false;
        float _firmwareVersion = 0;
    };
}

#endif // SOCKETCLIENT_H