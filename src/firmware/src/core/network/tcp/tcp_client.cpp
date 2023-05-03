#include "tcp_client.hpp"

#include "tcp_stream_helper.hpp"

namespace SyncBlink
{
    TcpClient::TcpClient(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _client.setNoDelay(true);
    }

    TcpClient::TcpClient(MessageBus& messageBus, WiFiClient client) : _messageBus(messageBus), _client(client)
    {
        _client.setNoDelay(true);
    }

    void TcpClient::start(String serverIp, uint16_t port)
    {
        _serverIp = serverIp;
        _port = port;

        checkConnection();
    }

    void TcpClient::stop()
    {
        _client.stop();
    }

    void TcpClient::flush()
    {
        _client.flush();
    }

    void TcpClient::loop()
    {
        checkConnection();
        handleIncomingServerMessages();
    }

    void TcpClient::writeFile(File file)
    {
        if (_client.connected())
        {
            long started = millis();
            size_t fileSize = file.size();
            while (fileSize > 0)
            {
                if (millis() - started > SocketWriteTimeout)
                {
                    _writeTimeout = true;
                    break;
                }

                uint32_t written = _client.write(file);
                fileSize -= written;
                if (fileSize > 0) delay(0);
            }
        }
    }

    void TcpClient::writeMessage(std::vector<uint8_t> message)
    {
        if (_client.connected())
        {
            long started = millis();
            uint8_t* messagePtr = &message[0];
            size_t messageSize = message.size();
            while (messageSize > 0)
            {
                if (millis() - started > SocketWriteTimeout)
                {
                    _writeTimeout = true;
                    break;
                }

                uint32_t written = _client.write(messagePtr, messageSize);
                messagePtr += written;
                messageSize -= written;
                if (messageSize > 0) delay(0);
            }
        }
    }

    MessagePackage TcpClient::writeBinaryUntilMessage(Stream& writeTo)
    {
        uint8_t magicBuf[2];
        MessagePackage message;
        while (_client.available())
        {                
            uint8_t byte = _client.read();
            if (byte == PacketMagicBytes[0])
            {
                magicBuf[0] = _client.read();
                magicBuf[1] = _client.read();
                if (magicBuf[0] == PacketMagicBytes[1] && magicBuf[1] == PacketMagicBytes[2])
                {
                    uint32_t messageSize = (_client.read() << 24) + (_client.read() << 16) + (_client.read() << 8) + _client.read();
                    uint8_t messageType = _client.read();

                    message.type = messageType;
                    message.body.resize(messageSize);

                    for (uint32_t byte = 0; byte < messageSize; byte++)
                    {
                        while (!_client.available())
                            yield();                         // Wait for new bytes
                        message.body[byte] = _client.read(); // then read
                    }
                    break;
                }
                else
                {
                    writeTo.write(magicBuf[0]);
                    writeTo.write(magicBuf[1]);
                }
            }
            else writeTo.write(byte);
        }
    }

    bool TcpClient::isConnected()
    {
        return _client.connected();
    }

    bool TcpClient::isDiscontinued()
    {
        return _retryCount >= 10;
    }

    bool TcpClient::isWriteTimeout()
    {
        return _writeTimeout;
    }

    void TcpClient::setStreamId(uint64_t id)
    {
        _streamId = id;
    }

    uint64_t TcpClient::getStreamId() const
    {
        return _streamId;
    }

    WiFiClient& TcpClient::getWiFiClient()
    {
        return _client;
    }

    IPAddress TcpClient::getRemoteIp()
    {
        return _client.remoteIP();
    }

    void TcpClient::checkConnection()
    {
        if (!_client.connected() && WiFi.status() == WL_CONNECTED && _retryCount++ < 10 && connectTo(_serverIp, 81))
        {
            _retryCount = 0;
        }
    }

    bool TcpClient::connectTo(String socketIp, uint16_t port)
    {
        bool connected = false;
        Serial.println("[TCP Client] Connecting to TCP '" + socketIp + "' ...");
        if (_client.connect(socketIp, 81))
        {
            Serial.println(F("[TCP Client] Connected!"));
            connected = true;
        }
        return connected;
    }

    void TcpClient::handleIncomingServerMessages()
    {
        MessagePackage package;
        if (TcpStreamHelper::messageAvailable(_client, package))
        {
            MessageBus::packageToBus(_messageBus, package);
        }
    }
}