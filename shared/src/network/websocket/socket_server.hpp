#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "deps/websockets/src/WebSocketsServer.h"
#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <map>

namespace SyncBlink
{
    typedef std::function<void()> MeshConnectionEvent;
    typedef std::function<void(Client::Message message)> ServerMessageEvent;

    struct WaitInfo
    {
        uint8_t receivedAnswers;
        Client::Message savedAnswer;
    };

    class SocketServer
    {
    public:
        SocketServer();

        void loop();
        void broadcast(Server::Message message);
        void broadcastMod(std::string &mod);

        uint32_t getClientsCount();

        EventRegistration<ServerMessageEvent> messageEvents;

    private:
        void serverEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
        void handleReceivedMessage(Client::Message receivedMessage);

        std::map<uint64_t, WaitInfo> _waitInfos;
        WebSocketsServer _webSocket = WebSocketsServer(81);
    };
}

#endif // SOCKETSERVER_H