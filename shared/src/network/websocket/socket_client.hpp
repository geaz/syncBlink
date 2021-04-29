#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "event_registration.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <functional>
#include <WebSocketsClient.h>

namespace SyncBlink
{
    typedef std::function<void(std::string mod)> MeshModEvent;
    typedef std::function<void(bool isConnected)> ClientConnectionEvent;

    typedef std::function<void(AudioAnalyzerMessage message)> AudioAnalyzerEvent;
    typedef std::function<void(Server::UpdateMessage message)> MeshUpdateEvent;
    typedef std::function<void(Server::SourceMessage message)> SourceUpdateEvent;
    typedef std::function<void(std::vector<uint8_t> data, Server::MessageType messageType)> FirmwareFlashEvent;

    class SocketClient
    {
    public:
        void start(String socketIp);
        void loop();
        void sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType);

        bool isConnected() const;

        EventRegistration<MeshModEvent> meshModEvents;
        EventRegistration<ClientConnectionEvent> connectionEvents;
        EventRegistration<AudioAnalyzerEvent> audioAnalyzerEvents;
        EventRegistration<MeshUpdateEvent> meshUpdateEvents;
        EventRegistration<SourceUpdateEvent> sourceUpdateEvents;
        EventRegistration<FirmwareFlashEvent> firmwareFlashEvents;

    private:
        void clientEvent(WStype_t type, uint8_t *payload, size_t length);

        WebSocketsClient _webSocket;
        bool _isConnected = false;
    };
}

#endif // SOCKETCLIENT_H