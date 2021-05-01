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
        #ifdef DEBUG_SOCKET
        Serial.printf("[TCP CLIENT] Writing message! Type: %i, Size: %i\n", messageType, messageSize);
        #endif
        _client.writeMessage(tcpMessage);
    }

    bool TcpClient::isConnected()
    {
        return _client.isConnected();
    }

    void TcpClient::checkConnection()
    {
        if(!_client.isConnected() && WiFi.status() == WL_CONNECTED)
        {
            #ifdef DEBUG_SOCKET
            Serial.println("[TCP CLIENT] Disconnected! Trying to reconnect ...");
            #endif
            if(_client.connectTo(_serverIp, 81))
            {
                #ifdef DEBUG_SOCKET
                Serial.println("[TCP CLIENT] Reconnected!");
                #endif
                if(!_wasConnected)
                {
                    for (auto event : connectionEvents.getEventHandlers())
                        event.second(true);
                    _wasConnected = true;
                }
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
                    Server::SourceMessage message;
                    memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : sourceUpdateEvents.getEventHandlers())
                        event.second(message);
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
                case Server::DISTRIBUTE_MOD:
                {
                    std::string mod((char*)&tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : meshModEvents.getEventHandlers())
                        event.second(mod);
                    break;
                }
                case Server::FIRMWARE_FLASH_START:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(std::vector<uint8_t>(), targetClientId, Server::FIRMWARE_FLASH_START);
                    break;
                }
                case Server::FIRMWARE_FLASH_END:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &tcpMessage.message[0], tcpMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(std::vector<uint8_t>(), targetClientId, Server::FIRMWARE_FLASH_END);
                    break;
                }
                case Server::FIRMWARE_FLASH_DATA:
                {
                    std::vector<uint8_t> data;
                    for(size_t i = 0; i < tcpMessage.message.size(); i++)
                    {
                        data.push_back(tcpMessage.message[i]);
                    }
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(data, 0, Server::FIRMWARE_FLASH_DATA);
                    break;
                }
            }
        }
    }
}