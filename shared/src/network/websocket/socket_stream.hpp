#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include <vector>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    struct SocketMessage
    {
        uint8_t messageType;
        std::vector<uint8_t> message;
    };

    class SocketStream
    {
    public:
        SocketStream();
        SocketStream(WiFiClient client);
    
        bool connectTo(String socketIp, uint16_t port);
        bool checkMessage(SocketMessage& message);
        void writeMessage(std::vector<uint8_t> message);

        bool isTimeout() const;
        bool isConnected();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        static std::vector<uint8_t> serializeMessage(void* message, uint32_t messageSize, uint8_t messageType);

    private:
        WiFiClient _client;
        bool _timeout = false;
        uint64_t _streamId = 0;
    };
}

#endif // SOCKETSTREAM_H