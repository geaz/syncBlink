#include "socket_server.hpp"
#include <iostream>

namespace SyncBlink
{
    SocketServer::SocketServer()
    {
        _webSocket.begin();
        _webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
            serverEvent(num, type, payload, length);
        });
    }

    void SocketServer::loop()
    {
        _webSocket.loop();
    }

    void SocketServer::broadcast(Server::Message message)
    {
        size_t messageSize = sizeof(message);
        uint8_t serializedMessage[messageSize];
        memcpy(&serializedMessage, &message, messageSize);

        switch (message.messageType)
        {
        case Server::MESH_COUNT_REQUEST:
            _waitFor = Client::MESH_COUNTED;
            _sendMessage = message;
            break;
        case Server::MESH_UPDATE:
            _waitFor = Client::MESH_UPDATED;
            _sendMessage = message;
            break;
        default:
            // No other message types are required to wait
            // Mod Distribution will be handled in "broadcastMod"
            break;
        }
        _webSocket.broadcastBIN(&serializedMessage[0], messageSize);
    }

    void SocketServer::broadcastMod(std::string& mod)
    {
        _waitFor = Client::MOD_DISTRIBUTED;
        _webSocket.broadcastTXT(mod.c_str(), mod.length());
    }

    uint32_t SocketServer::getClientsCount()
    {
        return _webSocket.connectedClients();
    }

    void SocketServer::serverEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length)
    {
        IPAddress remoteIp = _webSocket.remoteIP(num);
        switch (type)
        {
        case WStype_DISCONNECTED:
            Serial.println("Client disconnected!");
            for (auto event : meshConnectionEvents.getEventHandlers())
                event.second();
            break;
        case WStype_CONNECTED:
            Serial.println("Connection from: " + remoteIp.toString());
            for (auto event : meshConnectionEvents.getEventHandlers())
                event.second();
            break;
        case WStype_BIN:
            Client::Message receivedMessage;
            memcpy(&receivedMessage, payload, length);
            handleReceivedMessage(receivedMessage);
            break;
        default:
            break;
        }
    }

    void SocketServer::handleReceivedMessage(Client::Message receivedMessage)
    {        
        bool forwardMessage = false;
        if(_waitFor == receivedMessage.messageType && (_sendMessage.id == receivedMessage.id || _waitFor == Client::MOD_DISTRIBUTED))
        {
            _answers++;
            forwardMessage = _answers == _webSocket.connectedClients();

            switch (receivedMessage.messageType)
            {
            case Client::MESH_COUNTED:
                if (_savedMessage.id == 0)
                    _savedMessage = receivedMessage;
                else
                {
                    _savedMessage.countedMessage.totalLedCount += receivedMessage.countedMessage.totalLedCount;
                    _savedMessage.countedMessage.totalNodeCount += receivedMessage.countedMessage.totalNodeCount;
                    if (_savedMessage.countedMessage.routeLedCount < receivedMessage.countedMessage.routeLedCount)
                        _savedMessage.countedMessage.routeLedCount = receivedMessage.countedMessage.routeLedCount;
                    if (_savedMessage.countedMessage.routeNodeCount < receivedMessage.countedMessage.routeNodeCount)
                        _savedMessage.countedMessage.routeNodeCount = receivedMessage.countedMessage.routeNodeCount;
                }

                if(forwardMessage) // Last answer received
                {
                    _savedMessage.countedMessage.totalNodeCount++;
                    _savedMessage.countedMessage.totalLedCount += LED_COUNT;
                    _savedMessage.countedMessage.routeNodeCount++;
                    _savedMessage.countedMessage.routeLedCount += LED_COUNT;
                }
                break;
            default:
                _savedMessage = receivedMessage;
                break;
            }

            if (forwardMessage)
            {
                receivedMessage = _savedMessage;
                _answers = 0;
                _savedMessage.id = 0;
                _waitFor = SyncBlink::Client::NONE;
            }
        }
        else if(receivedMessage.messageType == Client::EXTERNAL_ANALYZER 
            ||receivedMessage.messageType == Client::MESH_CONNECTION)
        {
            forwardMessage = true;
        }

        if (forwardMessage)
        {
            for (auto event : messageEvents.getEventHandlers())
                event.second(receivedMessage);
        }
    }
}