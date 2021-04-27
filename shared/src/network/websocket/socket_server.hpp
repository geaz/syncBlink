#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "deps/websockets/src/WebSocketsServer.h"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <map>

namespace SyncBlink
{
    typedef std::function<void(uint64_t clientId)> ServerDisconnectionEvent;
    typedef std::function<void(Client::MessageType messageType, uint8_t* payload, size_t length)> ServerMessageEvent;

    class SocketServer
    {
    public:
        SocketServer();

        void loop();
        void broadcast(void* message, uint32_t messageSize, Server::MessageType messageType);

        uint32_t getClientsCount();

        EventRegistration<ServerDisconnectionEvent> serverDisconnectionEvents;
        EventRegistration<ServerMessageEvent> messageEvents;

    private:
        void serverEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

        std::map<uint8_t, uint64_t> _connectedClients;
        WebSocketsServer _webSocket = WebSocketsServer(81);
    };
}

#endif // SOCKETSERVER_H