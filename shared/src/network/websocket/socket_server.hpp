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
    typedef std::function<void(Client::Message message)> ServerMessageEvent;

    class SocketServer
    {
    public:
        SocketServer();

        void loop();
        void broadcast(Server::Message message);
        void broadcastMod(std::string &mod);

        uint32_t getClientsCount();

        EventRegistration<ServerDisconnectionEvent> serverDisconnectionEvents;
        EventRegistration<ServerMessageEvent> messageEvents;

    private:
        void serverEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
        void handleReceivedMessage(Client::Message receivedMessage, IPAddress clientIp);

        uint8_t _answers = 0;
        uint64_t _waitStartedAt = 0;
        Client::MessageType _waitFor;

        std::map<uint8_t, uint64_t> _connectedClients;
        WebSocketsServer _webSocket = WebSocketsServer(81);
    };
}

#endif // SOCKETSERVER_H