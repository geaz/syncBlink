#include "tcp_client.hpp"

#include "message.hpp"

#include <event/events/script_change_event.hpp>

namespace SyncBlink
{
    TcpClient::TcpClient(EventBus& eventBus, WiFiClient client) : _eventBus(eventBus), _client(client)
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
        handleIncomingMessages();
    }

    void TcpClient::sendMessage(void* message, uint32_t messageSize, EventType eventType)
    {
        auto messagePacket = Message::toMessagePacket(message, messageSize, eventType);
        writeMessage(messagePacket);
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
                    Serial.printf("[TCP Client] Write Timeout\n");
                    _writeTimeout = true;
                    break;
                }

                uint32_t written = _client.write(messagePtr, messageSize);
                messagePtr += written;
                messageSize -= written;
                if (messageSize > 0)
                    delay(0);
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
        if (!_client.connected() && WiFi.status() == WL_CONNECTED && _retryCount++ < 10)
        {
            if (_wasConnected)
                Serial.println("[TCP Client] Disconnected! Trying to connect ...");
            else
                Serial.println("[TCP Client] Trying to connect ...");

            if (connectTo(_serverIp, 81))
            {
                if (!_wasConnected)
                {
                    _wasConnected = true;
                }
                _retryCount = 0;
            }
        }
    }

    bool TcpClient::connectTo(String socketIp, uint16_t port)
    {
        bool connected = false;
        Serial.println("[TCP Client] Connecting to TCP '" + socketIp + "' ...");
        if (_client.connect(socketIp, 81))
        {
            Serial.println("[TCP Client] Connected!");
            connected = true;
        }
        return connected;
    }

    void TcpClient::handleIncomingMessages()
    {
        Message message;
        if (message.available(_client, message))
        {
            switch (message.type)
            {
            case EventType::ScriptChangeEvent:
                auto scriptChangeEvent = message.as<Events::ScriptChangeEvent>();
                _eventBus.trigger<Events::ScriptChangeEvent>(scriptChangeEvent);
                break;
            }
        }
    }
}