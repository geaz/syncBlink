#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "core/message/message.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/message_types.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/script_change.hpp"

#include <ESP8266WiFi.h>
#include <functional>

namespace SyncBlink
{
    class TcpClient
    {
    public:
        TcpClient(MessageBus& messageBus);
        TcpClient(MessageBus& messageBus, WiFiClient client);

        void start(String serverIp, uint16_t port);
        void stop();
        void flush();

        void loop();

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

        MessageBus& _messageBus;
        WiFiClient _client;
        String _serverIp;
        uint16_t _port;

        uint64_t _streamId = 0;
        uint8_t _retryCount = 0;
        bool _writeTimeout = false;
    };
}

#endif // TCPCLIENT_H