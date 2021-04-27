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

    void SocketServer::broadcast(void* message, uint32_t messageSize, Server::MessageType messageType)
    {
        uint8_t serializedMessage[messageSize+1];
        memcpy(&serializedMessage[1], message, messageSize);
        serializedMessage[0] = messageType;
        
        _webSocket.broadcastBIN(&serializedMessage[0], messageSize+1);
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
        {
            if(payload[0] == Client::MESH_CONNECTION)
            {
                Client::ConnectionMessage message;
                memcpy(&message, &payload[1], length - 1);
                
                if(message.parentId == 0)
                {
                    message.parentId = SyncBlink::getId();
                    memcpy(&payload[1], &message, sizeof(message));

                    for(uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++)
                    {
                        if(_webSocket.remoteIP(i) == remoteIp)
                        {
                            _connectedClients[i] = message.clientId;
                        }
                    }
                }
            }
            for (auto event : messageEvents.getEventHandlers())
                event.second((Client::MessageType)payload[0], &payload[1], length - 1);
            break;
        }
        default:
            break;
        }
    }
}