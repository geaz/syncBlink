#include "socket_client.hpp"
#include "network/mesh/syncblink_mesh.hpp"

namespace SyncBlink
{
    void SocketClient::start(String socketIp)
    {
        Serial.println("Connecting to socket '" + socketIp + "' ...");
        _webSocket.begin(socketIp, 81, "/");
        _webSocket.onEvent(
            [this](WStype_t type, uint8_t* payload, size_t length) { clientEvent(type, payload, length); });
        _webSocket.setReconnectInterval(5000);
    }

    void SocketClient::loop()
    {
        _webSocket.loop();
    }

    void SocketClient::sendMessage(void* message, uint32_t messageSize, Client::MessageType messageType)
    {
        uint8_t serializedMessage[messageSize+1];
        memcpy(&serializedMessage[1], message, messageSize);
        serializedMessage[0] = messageType;

        _webSocket.sendBIN(&serializedMessage[0], messageSize+1);
    }

    bool SocketClient::isConnected() const
    {
        return _isConnected;
    }

    void SocketClient::clientEvent(WStype_t type, uint8_t* payload, size_t length)
    {
        switch (type)
        {
        case WStype_DISCONNECTED:
            Serial.println("Disconnected from WebSocket!");
            for (auto event : connectionEvents.getEventHandlers())
                event.second(false);
            _isConnected = false;
            break;
        case WStype_CONNECTED:
        {
            Serial.println("Connected to WebSocket!");
            for (auto event : connectionEvents.getEventHandlers())
                event.second(true);
            _isConnected = true;
            break;
        }
        case WStype_BIN:
            switch((Server::MessageType)payload[0])
            {
                case Server::ANALYZER_UPDATE:
                {
                    AudioAnalyzerMessage message;
                    memcpy(&message, &payload[1], length-1);
                    for (auto event : audioAnalyzerEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::MESH_UPDATE:
                {
                    Server::UpdateMessage message;
                    memcpy(&message, &payload[1], length-1);
                    for (auto event : meshUpdateEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::SOURCE_UPDATE:
                {
                    Server::SourceMessage message;
                    memcpy(&message, &payload[1], length-1);
                    for (auto event : sourceUpdateEvents.getEventHandlers())
                        event.second(message);
                    break;
                }
                case Server::DISTRIBUTE_MOD:
                {
                    std::string mod((char*)&payload[1], length-1);
                    for (auto event : meshModEvents.getEventHandlers())
                        event.second(mod);
                    break;
                }
                case Server::FIRMWARE_FLASH_START:
                {
                    uint64_t targetClientId = 0;
                    memcpy(&targetClientId, &payload[1], length-1);
                    Serial.println("t: " + targetClientId);
                    if(targetClientId == SyncBlink::getId() || targetClientId == 0)
                    {
                        for (auto event : firmwareFlashEvents.getEventHandlers())
                            event.second(std::vector<uint8_t>(), Server::FIRMWARE_FLASH_START);
                    }
                    break;
                }
                case Server::FIRMWARE_FLASH_END:
                {
                    uint64_t targetClientId = 0;
                    Serial.println("t: " + targetClientId);
                    memcpy(&targetClientId, &payload[1], length-1);
                    if(targetClientId == SyncBlink::getId() || targetClientId == 0)
                    {
                        for (auto event : firmwareFlashEvents.getEventHandlers())
                            event.second(std::vector<uint8_t>(), Server::FIRMWARE_FLASH_END);
                    }
                    break;
                }
                case Server::FIRMWARE_FLASH_DATA:
                {
                    std::vector<uint8_t> data;
                    for(size_t i = 1; i < length; i++)
                    {
                        data.push_back(payload[i]);
                    }
                    for (auto event : firmwareFlashEvents.getEventHandlers())
                        event.second(data, Server::FIRMWARE_FLASH_DATA);
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}