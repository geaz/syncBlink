#include "tcp_client.hpp"
#include "tcp_stream_helper.hpp"

namespace SyncBlink
{
    TcpClient::TcpClient(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _client.setNoDelay(true);
    }

    void TcpClient::start(String serverIp, uint16_t port)
    {
        _serverIp = serverIp;
        _port = port;

        checkConnection();
    }

    void TcpClient::loop()
    {
        checkConnection();
        handleIncomingServerMessages();
    }

    void TcpClient::writeMessage(std::vector<uint8_t> message)
    {
        if (_client.connected())
        {
            long started = millis();
            uint8_t* messagePtr = &message[0];
            uint32_t messageSize = message.size();
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

    bool TcpClient::isConnected()
    {
        return _client.connected();
    }

    bool TcpClient::isDiscontinued()
    {
        return _retryCount >= 10;
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
        #ifdef DEBUG_TCP
        Serial.println("[TCP] Connecting to TCP '" + socketIp + "' ...");
        #endif
        bool connected = false;
        if (_client.connect(socketIp, 81))
        {
            #ifdef DEBUG_TCP
            Serial.println("[TCP] Connected!");
            #endif
            connected = true;
        }
        return connected;
    }

    void TcpClient::handleIncomingServerMessages()
    {
        MessagePackage package;
        if (TcpStreamHelper::messageAvailable(_client, package))
        {
            #ifdef DEBUG_TCP
            Serial.println("[TCP] Got message!");                   
            #endif
            MessageBus::packageToBus(_messageBus, package);
        }
    }
}