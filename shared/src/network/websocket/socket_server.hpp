#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "socket_stream.hpp"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <vector>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    typedef std::function<void(uint64_t clientId)> ServerDisconnectionEvent;
    typedef std::function<void(SocketMessage message)> ServerMessageEvent;

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
        void clearClients();
        void checkNewClients();
        void handleIncomingMessages();

        WiFiServer _server = WiFiServer(81);
        std::vector<SocketStream> _clients;
    };
}

#endif // SOCKETSERVER_H