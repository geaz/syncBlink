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
            _waitInfos[message.id] = {0};
            break;
        case Server::MESH_UPDATE:            
            _waitInfos[message.id] = {0};
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
        _waitInfos[0] = {0};
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
        auto iter = _waitInfos.find(receivedMessage.id);
        if(iter != _waitInfos.end())
        {
            auto& waitInfo = iter->second;
            waitInfo.receivedAnswers++;
            forwardMessage = waitInfo.receivedAnswers == _webSocket.connectedClients();

            switch (receivedMessage.messageType)
            {
            case SyncBlink::Client::MESH_COUNTED:
                if (waitInfo.savedAnswer.id == 0)
                    waitInfo.savedAnswer = receivedMessage;
                else
                {
                    waitInfo.savedAnswer.countedMessage.totalLedCount += receivedMessage.countedMessage.totalLedCount;
                    waitInfo.savedAnswer.countedMessage.totalNodeCount += receivedMessage.countedMessage.totalNodeCount;
                    if (waitInfo.savedAnswer.countedMessage.routeLedCount < receivedMessage.countedMessage.routeLedCount)
                        waitInfo.savedAnswer.countedMessage.routeLedCount = receivedMessage.countedMessage.routeLedCount;
                    if (waitInfo.savedAnswer.countedMessage.routeNodeCount < receivedMessage.countedMessage.routeNodeCount)
                        waitInfo.savedAnswer.countedMessage.routeNodeCount = receivedMessage.countedMessage.routeNodeCount;
                }
                break;
            default:
                waitInfo.savedAnswer = receivedMessage;
                break;
            }

            if (forwardMessage)
            {
                receivedMessage = waitInfo.savedAnswer;
                _waitInfos.erase(iter->first);
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