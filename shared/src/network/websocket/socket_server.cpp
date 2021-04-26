#include "socket_server.hpp"
#include "network/mesh/syncblink_mesh.hpp"

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

        switch(message.messageType)
        {
            case Server::MESH_UPDATE:
                _waitFor = Client::MESH_UPDATED;
                _waitStartedAt = millis();
                break;
            case Server::SOURCE_UPDATE:
            case Server::ANALYZER_UPDATE:
                break;
        }            
        
        _webSocket.broadcastBIN(&serializedMessage[0], messageSize);
    }

    void SocketServer::broadcastMod(std::string& mod)
    {
        _waitStartedAt = millis();
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
            for (auto event : serverDisconnectionEvents.getEventHandlers())
                event.second(_connectedClients[num]);
            _connectedClients[num] = 0;
            break;
        case WStype_BIN:
            Client::Message receivedMessage;
            memcpy(&receivedMessage, payload, length);
            handleReceivedMessage(receivedMessage, remoteIp);
            break;
        default:
            break;
        }
    }

    void SocketServer::handleReceivedMessage(Client::Message receivedMessage, IPAddress clientIp)
    {
        bool forwardMessage = false;

        if(_waitStartedAt != 0 && _waitFor == receivedMessage.messageType)
        {
            forwardMessage = ++_answers == _webSocket.connectedClients();
            if (forwardMessage)
            {
                _answers = 0;
                _waitStartedAt = 0;
            }
        }
        else if(receivedMessage.messageType == Client::MESH_CONNECTION)
        {
            forwardMessage = true;
            if(receivedMessage.connectionMessage.parentId == 0)
            {
                receivedMessage.connectionMessage.parentId = SyncBlink::getId();
                for(uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
                {
                    if(_webSocket.remoteIP(i) == clientIp)
                    {
                        _connectedClients[i] = receivedMessage.connectionMessage.clientId;
                    }
                }
            }
        }
        else if(receivedMessage.messageType == Client::EXTERNAL_ANALYZER || receivedMessage.messageType == Client::MESH_DISCONNECTION)
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