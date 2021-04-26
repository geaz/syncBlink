#include "socket_client.hpp"

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

    void SocketClient::sendMessage(Client::Message message)
    {
        size_t messageSize = sizeof(message);
        uint8_t serializedMessage[messageSize];
        memcpy(&serializedMessage, &message, messageSize);

        _webSocket.sendBIN(&serializedMessage[0], messageSize);
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
            Server::Message receivedMessage;
            memcpy(&receivedMessage, payload, length);
            for (auto event : messageEvents.getEventHandlers())
                event.second(receivedMessage);
            break;
        case WStype_TEXT:
            for (auto event : meshModEvents.getEventHandlers())
                event.second(std::string((char*)payload));
            break;
        default:
            break;
        }
    }
}