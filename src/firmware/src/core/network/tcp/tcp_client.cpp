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
            Serial.println("[TCP Client] Connected!");
            connected = true;
        }
        return connected;
    }

    void TcpClient::handleIncomingServerMessages()
    {
        MessagePackage package;
        if (TcpStreamHelper::messageAvailable(_client, package))
        {
            switch (package.type)
            {
            case MessageType::ScriptChange: {
                Messages::ScriptChange scriptChangeMsg;
                scriptChangeMsg.loadPackage(package);

                _messageBus.trigger(scriptChangeMsg);
                break;
            }
            case MessageType::AnalyzerUpdate: {
                Messages::AnalyzerUpdate analyzerUpdateMsg;
                analyzerUpdateMsg.loadPackage(package);

                _messageBus.trigger(analyzerUpdateMsg);
                break;
            }
            case MessageType::MeshUpdate: {
                Messages::MeshUpdate meshUpdateMsg;
                meshUpdateMsg.loadPackage(package);

                _messageBus.trigger(meshUpdateMsg);
                break;
            }
            case MessageType::NodeCommand: {
                Messages::NodeCommand nodeCommandMsg;
                nodeCommandMsg.loadPackage(package);

                _messageBus.trigger(nodeCommandMsg);
                break;
            }
            }
        }
    }
}