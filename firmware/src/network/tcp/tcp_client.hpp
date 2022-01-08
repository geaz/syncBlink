#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <functional>
#include <ESP8266WiFi.h>

#include <event/event_bus.hpp>
#include <event/event_types.hpp>

namespace SyncBlink
{
    class TcpClient
    {
    public:
        TcpClient(EventBus& eventBus, WiFiClient client);

        void start(String serverIp, uint16_t port);
        void stop();
        void flush();

        void loop();
                
        void sendMessage(void* message, uint32_t messageSize, EventType eventType);
        void writeMessage(std::vector<uint8_t> message);

        bool isConnected();
        bool isDiscontinued();
        bool isWriteTimeout();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        WiFiClient& getWiFiClient();
        IPAddress getRemoteIp();

    private:
        void checkConnection();
        bool connectTo(String socketIp, uint16_t port);
        void handleIncomingMessages();

        EventBus& _eventBus;
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