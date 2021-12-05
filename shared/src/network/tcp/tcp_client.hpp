#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "event_registration.hpp"
#include "messages/message.hpp"
#include "messages/client_messages.hpp"
#include "messages/server_messages.hpp"

#include <functional>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    typedef std::function<void(std::string script)> MeshScriptEvent;
    typedef std::function<void(bool isConnected)> ClientConnectionEvent;

    typedef std::function<void(uint64_t nodeId)> PingEvent;
    typedef std::function<void(bool lightMode)> LightModeEvent;
    typedef std::function<void(AudioAnalyzerMessage message)> AudioAnalyzerEvent;
    typedef std::function<void(Server::UpdateMessage message)> MeshUpdateEvent;
    typedef std::function<void(uint64_t nodeId)> SourceUpdateEvent;
    typedef std::function<void(Server::NodeRenameMessage message)> NodeRenameEvent;
    typedef std::function<void(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType)> FirmwareFlashEvent;

    class TcpClient
    {
    public:
        TcpClient();
        TcpClient(WiFiClient client);

        void start(String serverIp, uint16_t port);
        void stop();
        void flush();

        void loop();
                
        void sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType);
        void writeMessage(std::vector<uint8_t> message);

        bool isConnected();
        bool isDiscontinued();
        bool isWriteTimeout();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        WiFiClient& getWiFiClient();
        IPAddress getRemoteIp();

        EventRegistration<PingEvent> pingEvents;
        EventRegistration<LightModeEvent> lightModeEvents;
        EventRegistration<MeshScriptEvent> meshScriptEvents;
        EventRegistration<ClientConnectionEvent> connectionEvents;
        EventRegistration<AudioAnalyzerEvent> audioAnalyzerEvents;
        EventRegistration<MeshUpdateEvent> meshUpdateEvents;
        EventRegistration<SourceUpdateEvent> sourceUpdateEvents;
        EventRegistration<NodeRenameEvent> nodeRenameEvents;
        EventRegistration<FirmwareFlashEvent> firmwareFlashEvents;

    private:
        void checkConnection();
        bool connectTo(String socketIp, uint16_t port);
        void handleIncomingMessages();

        WiFiClient _client;
        String _serverIp;
        uint16_t _port;

        uint8_t _retryCount = 0;
        bool _wasConnected = false;
        uint64_t _streamId = 0;
        bool _writeTimeout = false;
    };
}

#endif // TCPCLIENT_H