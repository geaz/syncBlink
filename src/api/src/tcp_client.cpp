#include "tcp_client.hpp"

#include <analyzer_change.hpp>
#include <inttypes.h>
#include <iostream>
#include <thread>

namespace SyncBlink
{
    namespace Api
    {
        TcpClient::TcpClient(MessageBus& messageBus) : _messageBus(messageBus), _socket(_ioContext), _resolver(_ioContext)
        {
        }

        TcpClient::~TcpClient()
        {
            _socket.shutdown(asio::socket_base::shutdown_both);
            _socket.close();
        }

        void TcpClient::start(const std::string tcpServerIp)
        {
            if (_connected) return;

            for (auto event : infoMessageEvents.getEventHandlers())
                event.second("Starting TCP Client ...");

            _socket.async_connect(_resolver.resolve(tcpServerIp, "81")->endpoint(), [this](const asio::error_code& err) {
                if (err.value() != 0)
                {
                    for (auto event : errorMessageEvents.getEventHandlers())
                        event.second(err.message());
                    _socket.close();
                }
                else
                {
                    _connected = true;
                    for (auto event : connectionEvents.getEventHandlers())
                        event.second(_connected);
                    startRead();
                }
            });

            // run asio async in the background
            _ioThread = std::thread([this]() {
                _ioContext.run();
                _ioContext.reset();
            });

            _ioThread.detach();
        }

        void TcpClient::writeMessage(std::vector<uint8_t> message)
        {
            if (!_connected) return;

            uint8_t* messagePtr = &message[0];
            size_t messageSize = message.size();
            asio::async_write(_socket, asio::buffer(messagePtr, messageSize), [this](const asio::error_code& err, std::size_t n) {
                if (err) handleDisconnect(err);
            });
        }

        void TcpClient::startRead()
        {
            asio::async_read(_socket, asio::buffer(_singleByte, 1), [this](const asio::error_code& err, std::size_t n) {
                if (err) handleDisconnect(err);
                else
                {
                    if (_singleByte[0] == PacketMagicBytes[0])
                    {
                        uint8_t magicBuf[2];
                        size_t readLen = asio::read(_socket, asio::buffer(magicBuf, 2));
                        if (readLen == sizeof(magicBuf) && magicBuf[0] == PacketMagicBytes[1] && magicBuf[1] == PacketMagicBytes[2])
                        {
                            uint8_t messageHeader[5];
                            readLen = asio::read(_socket, asio::buffer(messageHeader, 5));
                            if (readLen == sizeof(messageHeader))
                            {
                                uint32_t messageSize =
                                    (messageHeader[0] << 24) + (messageHeader[1] << 16) + (messageHeader[2] << 8) + messageHeader[3];
                                uint8_t messageType = messageHeader[4];

                                MessagePackage package;
                                package.type = messageType;
                                package.body.resize(messageSize);

                                size_t readBytes = 0;
                                while (readBytes < messageSize)
                                {
                                    readBytes += asio::read(_socket, asio::buffer(&package.body[readBytes], messageSize - readBytes));
                                }
                                // printf("Found message - Size: %zi, Type: %i\n", package.body.size() + PacketHeaderSize, package.type);

                                switch (package.type)
                                {
                                case MessageType::AnalyzerChange: {
                                    Messages::AnalyzerChange analyzerChangeMsg;
                                    analyzerChangeMsg.loadPackage(package);

                                    _messageBus.trigger(analyzerChangeMsg);
                                    break;
                                }
                                    // We are not interested in any other event from the mesh at the moment.
                                }
                            }
                        }
                    }
                    if (_connected) startRead();
                }
            });
        }

        void TcpClient::handleDisconnect(const asio::error_code& err)
        {
            _connected = false;
            _socket.shutdown(asio::socket_base::shutdown_both);
            _socket.close();

            for (auto event : connectionEvents.getEventHandlers())
                event.second(_connected);
            for (auto event : errorMessageEvents.getEventHandlers())
                event.second(err.message());
        }
    }
}