#include <iostream>
#include <inttypes.h>
#include <analyzer_change.hpp>

#include "tcp_client.hpp"

namespace SyncBlink
{
    namespace Api
    {
        TcpClient::TcpClient(const std::string tcpServerIp, MessageBus& messageBus) : 
            _tcpServerIp(tcpServerIp), _messageBus(messageBus), _socket(_ioContext), _resolver(_ioContext)
        {}

        void TcpClient::start(std::vector<uint8_t> connectionMessage)
        {
            std::cout << "Connecting...\n";
            _socket.async_connect(_resolver.resolve(_tcpServerIp, "81")->endpoint(), [this, connectionMessage](const asio::error_code& ec) {                
                if (ec)
                {
                    std::cout << "Connect error: " << ec.message() << "\n";
                    _socket.close();
                }
                else
                {
                    std::cout << "Connected!\n";
                    _connected = true;

                    writeMessage(connectionMessage);
                    startRead();
                }
            });
            _ioContext.run();
        }

        void TcpClient::stop()
        {
            asio::error_code ignored_ec;
            _socket.close(ignored_ec);
            _connected = false;
        }

        void TcpClient::writeMessage(std::vector<uint8_t> message)
        {
            if (_connected)
            {
                uint8_t* messagePtr = &message[0];
                size_t messageSize = message.size();
                asio::write(_socket, asio::buffer(messagePtr, messageSize));
            }
        }

        void TcpClient::startRead()
        {
            asio::async_read(_socket, asio::buffer(_singleByte, 1), [this](const asio::error_code& ec, std::size_t n) {                
                if (ec)
                {
                    std::cout << "Error: " << ec.message() << "\n";
                    _socket.close();
                }
                else
                {
                    if(_singleByte[0] == PacketMagicBytes[0])
                    {
                        uint8_t magicBuf[2];
                        size_t readLen = asio::read(_socket, asio::buffer(magicBuf, 2));
                        if(readLen == sizeof(magicBuf) && magicBuf[0] == PacketMagicBytes[1] && magicBuf[1] == PacketMagicBytes[2])
                        {
                            uint8_t messageHeader[5];
                            readLen = asio::read(_socket, asio::buffer(messageHeader, 5));
                            if(readLen == sizeof(messageHeader))
                            {
                                uint32_t messageSize = 
                                    (messageHeader[0]<<24) + 
                                    (messageHeader[1]<<16) + 
                                    (messageHeader[2]<<8) + 
                                    messageHeader[3];
                                uint8_t messageType = messageHeader[4];

                                MessagePackage package;
                                package.type = messageType;
                                package.body.resize(messageSize);

                                size_t readBytes = 0;
                                while(readBytes < messageSize)
                                {
                                    readBytes += asio::read(_socket, asio::buffer(&package.body[readBytes], messageSize-readBytes));
                                }
                                //printf("Found message - Size: %zi, Type: %i\n", package.body.size() + PacketHeaderSize, package.type);

                                switch (package.type)
                                {
                                case MessageType::AnalyzerChange: {
                                    Messages::AnalyzerChange analyzerChangeMsg;
                                    analyzerChangeMsg.loadPackage(package);

                                    _messageBus.trigger(analyzerChangeMsg);
                                    break;
                                }
                                }
                            }
                        }
                    }
                    startRead();
                }                
            });
        }
    }
}