#ifndef SYNCBLINKDESKTOPTCPCLIENT_H
#define SYNCBLINKDESKTOPTCPCLIENT_H

#include "event_registration.hpp"

#include <asio.hpp>
#include <functional>
#include <message_bus.hpp>
#include <vector>

namespace SyncBlink
{
    namespace Api
    {
        typedef std::function<void(std::string message)> ErrorMessageEvent;
        typedef std::function<void(std::string message)> InfoMessageEvent;
        typedef std::function<void(bool isConnected)> ConnectionEvent;

        class TcpClient
        {
        public:
            TcpClient(MessageBus& messageBus);
            ~TcpClient();

            void start(const std::string tcpServerIp);
            void writeMessage(std::vector<uint8_t> message);

            EventRegistration<ErrorMessageEvent> errorMessageEvents;
            EventRegistration<InfoMessageEvent> infoMessageEvents;
            EventRegistration<ConnectionEvent> connectionEvents;

        private:
            void startRead();

            MessageBus& _messageBus;

            std::thread _ioThread;
            asio::io_context _ioContext;
            asio::ip::tcp::socket _socket;
            asio::ip::tcp::resolver _resolver;

            uint8_t _singleByte[1];
            bool _connected = false;
        };
    }
}

#endif // SYNCBLINKDESKTOPTCPCLIENT_H