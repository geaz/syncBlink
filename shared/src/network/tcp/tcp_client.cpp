#include "tcp_client.hpp"

namespace SyncBlink
{
    void TcpClient::start(String serverIp)
    {
        _serverIp = serverIp;
        checkConnection();
    }

    void TcpClient::loop()
    {
        checkConnection();
        handleIncomingMessages();
    }

    void TcpClient::sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType)
    {
        auto tcpMessage = TcpStream::serializeMessage(message, messageSize, messageType);
        #ifdef DEBUG_TCP
        Serial.printf("[TCP CLIENT] Writing message! Type: %i, Size: %i\n", messageType, messageSize);
        #endif
        _client.writeMessage(tcpMessage);
    }

    bool TcpClient::isConnected()
    {
        return _client.isConnected();
    }

    bool TcpClient::isDiscontinued()
    {
        return _retryCount >= 10;
    }

    void TcpClient::checkConnection()
    {
        if(!_client.isConnected() && WiFi.status() == WL_CONNECTED && _retryCount++ < 10)
        {
            #ifdef DEBUG_TCP
            Serial.println("[TCP CLIENT] Disconnected! Trying to connect ...");
            #endif
            if(_client.connectTo(_serverIp, 81))
            {
                #ifdef DEBUG_TCP
                Serial.println("[TCP CLIENT] Connected!");
                #endif
                for (auto event : connectionEvents.getEventHandlers())
                    event.second(true);
                _retryCount = 0;
            }
        }
    }

    void TcpClient::handleIncomingMessages()
    {
        TcpMessage tcpMessage;
        if(_client.checkMessage(tcpMessage))
        {
            switch(tcpMessage.messageType)
            {
                case Server::ANALYZER_UPDATE:
                {
                    AudioAnalyzerMessage message;
                    memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : audioAnalyzerEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::MESH_UPDATE:
                {
                    Server::UpdateMessage message;
                    memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : meshUpdateEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::SOURCE_UPDATE:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : sourceUpdateEvents.getEventHandlers())
                        event.second(targetClientId);
                    break;
                }
                case Server::NODE_RENAME:
                {
                    Server::NodeRenameMessage message;
                    memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : nodeRenameEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::DISTRIBUTE_SCRIPT:
                {
                    std::string script((char*)&tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : meshScriptEvents.getEventHandlers())
                        event.second(script);
                    break;
                }
                case Server::FIRMWARE_FLASH_START:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(tcpMessage.message, targetClientId, Server::FIRMWARE_FLASH_START);
                    break;
                }
                case Server::FIRMWARE_FLASH_END:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(tcpMessage.message, targetClientId, Server::FIRMWARE_FLASH_END);
                    break;
                }
                case Server::FIRMWARE_FLASH_DATA:
                {
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(tcpMessage.message, 0, Server::FIRMWARE_FLASH_DATA);
                    break;
                }
                case Server::PING:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : pingEvents.getEventHandlers())
                        event.second(targetClientId);
                    break;
                }
            }
        }
    }
}