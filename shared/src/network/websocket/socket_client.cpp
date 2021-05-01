#include "socket_client.hpp"

namespace SyncBlink
{
    void SocketClient::start(String serverIp)
    {
        _serverIp = serverIp;
        checkConnection();
    }

    void SocketClient::loop()
    {
        checkConnection();
        handleIncomingMessages();
    }

    void SocketClient::sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType)
    {
        auto socketMessage = SocketStream::serializeMessage(message, messageSize, messageType);
        #ifdef DEBUG_SOCKET
        Serial.printf("[TCP CLIENT] Writing message! Type: %i, Size: %i\n", messageType, messageSize);
        #endif
        _client.writeMessage(socketMessage);
    }

    bool SocketClient::isConnected()
    {
        return _client.isConnected();
    }

    void SocketClient::checkConnection()
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

    void SocketClient::handleIncomingMessages()
    {
        SocketMessage socketMessage;
        if(_client.checkMessage(socketMessage))
        {
            switch(socketMessage.messageType)
            {
                case Server::ANALYZER_UPDATE:
                {
                    AudioAnalyzerMessage message;
                    memcpy(&message, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : audioAnalyzerEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::MESH_UPDATE:
                {
                    Server::UpdateMessage message;
                    memcpy(&message, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : meshUpdateEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::SOURCE_UPDATE:
                {
                    Server::SourceMessage message;
                    memcpy(&message, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : sourceUpdateEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::NODE_RENAME:
                {
                    Server::NodeRenameMessage message;
                    memcpy(&message, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : nodeRenameEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::DISTRIBUTE_MOD:
                {
                    std::string mod((char*)&socketMessage.message[0], socketMessage.message.size());
                    for (auto event : meshModEvents.getEventHandlers())
                        event.second(mod);
                    break;
                }
                case Server::FIRMWARE_FLASH_START:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(std::vector<uint8_t>(), targetClientId, Server::FIRMWARE_FLASH_START);
                    break;
                }
                case Server::FIRMWARE_FLASH_END:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &socketMessage.message[0], socketMessage.message.size());
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(std::vector<uint8_t>(), targetClientId, Server::FIRMWARE_FLASH_END);
                    break;
                }
                case Server::FIRMWARE_FLASH_DATA:
                {
                    std::vector<uint8_t> data;
                    for(size_t i = 0; i < socketMessage.message.size(); i++)
                    {
                        data.push_back(socketMessage.message[i]);
                    }
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(data, 0, Server::FIRMWARE_FLASH_DATA);
                    break;
                }
            }
        }
    }
}