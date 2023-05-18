#ifndef SYNCBLINKDESKTOPTCPCLIENT_H
#define SYNCBLINKDESKTOPTCPCLIENT_H

#include <vector>
#include <functional>
#include <asio.hpp>
#include <message_bus.hpp>

namespace SyncBlink
{
    namespace Api
    {
        class TcpClient
        {
            public:
                TcpClient(const std::string tcpServerIp, MessageBus& messageBus);

                void start(std::vector<uint8_t> connectionMessage);
                void stop();
                void writeMessage(std::vector<uint8_t> message);

            private:
                void startRead();

                MessageBus& _messageBus;

                std::string _tcpServerIp;
                asio::io_context _ioContext;
                asio::ip::tcp::socket _socket;
                asio::ip::tcp::resolver _resolver;

                uint8_t _singleByte[1];
                bool _connected = false;
        };
    }
}

#endif // SYNCBLINKDESKTOPTCPCLIENT_H