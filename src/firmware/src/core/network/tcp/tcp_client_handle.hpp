#ifndef TCPCLIENTHANDLE_H
#define TCPCLIENTHANDLE_H

#include "core/message/message.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/message_types.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/script_change.hpp"
#include "core/message/messages/node_command.hpp"

#include <ESP8266WiFi.h>
#include <functional>

namespace SyncBlink
{
    class TcpClientHandle
    {
    public:
        TcpClientHandle(WiFiClient client);

        void writeMessage(std::vector<uint8_t> message);
        void stop();
        void flush();

        bool isConnected();
        bool isWriteTimeout();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        WiFiClient& getWiFiClient();
        IPAddress getRemoteIp();

    private:
        WiFiClient _client;

        uint64_t _streamId = 0;
        bool _writeTimeout = false;
    };
}

#endif // TCPCLIENTHANDLE_H