#include "tcp_client.hpp"

namespace SyncBlink
{
    TcpClient::TcpClient()
    {
        _client.setNoDelay(true);
    }

    TcpClient::TcpClient(WiFiClient client) : _client(client)
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

    void TcpClient::sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType)
    {
        auto messagePacket = Message::toMessagePacket(message, messageSize, messageType);
        #ifdef DEBUG_TCP
        Serial.printf("[TCP CLIENT] Sending message to server! Type: %i, Size: %i\n", messageType, messagePacket.size());
        #endif
        writeMessage(messagePacket);
    }

    void TcpClient::writeMessage(std::vector<uint8_t> message)
    {
        if(_client.connected())
        {
            long started = millis();
            uint8_t* messagePtr = &message[0];
            uint32_t messageSize = message.size();
            while(messageSize > 0)
            {
                if(millis() - started > SocketWriteTimeout)
                {
                    #ifdef DEBUG_TCP
                    Serial.printf("[TCP Client] Write Timeout\n");
                    #endif
                    _writeTimeout = true;
                    break;
                }
                
                uint32_t written = _client.write(messagePtr, messageSize);
                messagePtr += written;
                messageSize -= written;
                if(messageSize > 0) delay(0);
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
        if(!_client.connected() && WiFi.status() == WL_CONNECTED && _retryCount++ < 10)
        {
            #ifdef DEBUG_TCP
            if(_wasConnected)
                Serial.println("[TCP] Disconnected! Trying to connect ...");
            else
                Serial.println("[TCP] Trying to connect ...");
            #endif
            if(connectTo(_serverIp, 81))
            {
                if(!_wasConnected)
                {
                    _wasConnected = true;
                    for (auto event : connectionEvents.getEventHandlers())
                        event.second(true);
                }
                _retryCount = 0;
            }
        }
    }

    bool TcpClient::connectTo(String socketIp, uint16_t port)
    {
        bool connected = false;
        #ifdef DEBUG_TCP
        Serial.println("[TCP] Connecting to TCP '" + socketIp + "' ...");
        #endif
        if(_client.connect(socketIp, 81)){
            #ifdef DEBUG_TCP
            Serial.println("[TCP] Connected!");
            #endif
            connected = true;
        }
        return connected;
    }

    void TcpClient::handleIncomingMessages()
    {
        Message message;
        if(Message::available(_client, message))
        {
            switch(message.type)
            {
                case Server::ANALYZER_UPDATE:
                {
                    AudioAnalyzerMessage anaMessage;
                    memcpy(&anaMessage, &message.body[0], message.body.size());
                    for (auto event : audioAnalyzerEvents.getEventHandlers())
                        event.second(anaMessage);
                    break;
                }
                case Server::MESH_UPDATE:
                {
                    Server::UpdateMessage updateMessage;
                    memcpy(&updateMessage, &message.body[0], message.body.size());
                    for (auto event : meshUpdateEvents.getEventHandlers())
                        event.second(updateMessage);
                    break;
                }
                case Server::SOURCE_UPDATE:
                {
                    uint64_t targetNodeId = 0;
                    memcpy(&targetNodeId, &message.body[0], message.body.size());
                    for (auto event : sourceUpdateEvents.getEventHandlers())
                        event.second(targetNodeId);
                    break;
                }
                case Server::NODE_RENAME:
                {
                    Server::NodeRenameMessage renameMessage;
                    memcpy(&renameMessage, &message.body[0], message.body.size());
                    for (auto event : nodeRenameEvents.getEventHandlers())
                        event.second(renameMessage);
                    break;
                }
                case Server::DISTRIBUTE_SCRIPT:
                {
                    std::string script((char*)&message.body[0], message.body.size());
                    for (auto event : meshScriptEvents.getEventHandlers())
                        event.second(script);
                    break;
                }
                case Server::FIRMWARE_FLASH_START:
                {
                    uint64_t targetNodeId = 0;
                    memcpy(&targetNodeId, &message.body[0], message.body.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(message.body, targetNodeId, Server::FIRMWARE_FLASH_START);
                    break;
                }
                case Server::FIRMWARE_FLASH_END:
                {
                    uint64_t targetNodeId = 0;
                    memcpy(&targetNodeId, &message.body[0], message.body.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(message.body, targetNodeId, Server::FIRMWARE_FLASH_END);
                    break;
                }
                case Server::FIRMWARE_FLASH_DATA:
                {
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(message.body, 0, Server::FIRMWARE_FLASH_DATA);
                    break;
                }
                case Server::PING_NODE:
                {
                    uint64_t targetNodeId = 0;
                    memcpy(&targetNodeId, &message.body[0], message.body.size());
                    for (auto event : pingEvents.getEventHandlers())
                        event.second(targetNodeId);
                    break;
                }
                case Server::LIGHT_MODE:
                {
                    bool lightMode = false;
                    memcpy(&lightMode, &message.body[0], message.body.size());
                    for (auto event : lightModeEvents.getEventHandlers())
                        event.second(lightMode);
                    break;
                }
            }
        }
    }
}