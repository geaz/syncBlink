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
    
        bool connectTo(String socketIp, uint16_t port);
        bool checkMessage(TcpMessage& message);
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

#endif // TCPSTREAM_H