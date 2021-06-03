#ifndef TCPSTREAM_H
#define TCPSTREAM_H

#include <vector>
#include <ESP8266WiFi.h>

namespace SyncBlink
{
    struct TcpMessage
    {
        uint8_t messageType;
        std::vector<uint8_t> message;
    };

    class TcpStream
    {
    public:
        TcpStream();
        TcpStream(WiFiClient client);
    
        void stop();
        void flush();
        bool connectTo(String socketIp, uint16_t port);
        bool checkMessage(TcpMessage& message);
        void writeMessage(std::vector<uint8_t> message);

        bool isWriteTimeout();
        bool isConnected();

        void setStreamId(uint64_t id);
        uint64_t getStreamId() const;

        IPAddress getRemoteIp() const;

        static std::vector<uint8_t> serializeMessage(void* message, uint32_t messageSize, uint8_t messageType);

    private:
        WiFiClient _client;
        IPAddress _remoteIp;
        uint64_t _streamId = 0;
        bool _writeTimeout = false;
    };
}

#endif // TCPSTREAM_H