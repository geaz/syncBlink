#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "deps/websockets/src/WebSocketsServer.h"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

namespace SyncBlink
{
    typedef std::function<void()> MeshConnectionEvent;
    typedef std::function<void(Client::Message message)> ServerMessageEvent;

    class SocketServer
    {
    public:
        SocketServer();

        void loop();
        void broadcast(Server::Message message);
        void broadcastMod(std::string &mod);

        uint32_t getClientsCount();

        EventRegistration<MeshConnectionEvent> meshConnectionEvents;
        EventRegistration<ServerMessageEvent> messageEvents;

    private:
        void serverEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
        void handleReceivedMessage(Client::Message receivedMessage);

        uint16_t _answers = 0;
        Client::MessageType _waitFor;
        Server::Message _sendMessage;
        Client::Message _savedMessage = {0};

        WebSocketsServer _webSocket = WebSocketsServer(81);
    };
}

#endif // SOCKETSERVER_H