#include "socket_server.hpp"
#include "network/mesh/syncblink_mesh.hpp"

namespace SyncBlink
{
    SocketServer::SocketServer() { _server.begin(); }

    void SocketServer::loop()
    {
        clearClients();
        checkNewClients();
        handleIncomingMessages();
    }

    void SocketServer::broadcast(void* message, uint32_t messageSize, Server::MessageType messageType)
    {
        auto socketMessage = SocketStream::serializeMessage(message, messageSize, messageType);
        #ifdef DEBUG_SOCKET
        if(_clients.size() > 0) Serial.printf("[TCP SERVER] Writing message - Type: %i, Size: %i\n", messageType, socketMessage.size());
        #endif
        for(auto& client : _clients)
        {
            client.writeMessage(socketMessage);
        }
    }

    uint32_t SocketServer::getClientsCount()
    {
        return _clients.size();
    }

    void SocketServer::clearClients()
    {
        for(auto iter = _clients.begin(); iter != _clients.end();)
        {
            if(!iter->isConnected() || iter->isTimeout())
            {
                #ifdef DEBUG_SOCKET
                Serial.printf("[TCP SERVER] Client lost connection: %12llx\n", iter->getStreamId());
                #endif
                if(iter->getStreamId() != 0)
                {
                    for (auto event : serverDisconnectionEvents.getEventHandlers())
                        event.second(iter->getStreamId());
                }
                _clients.erase(iter);
            }
            else iter++;
        }
    }

    void SocketServer::checkNewClients()
    {
        if(_server.hasClient())
        {
            _clients.push_back(SocketStream(_server.available()));
        }
    }

    void SocketServer::handleIncomingMessages()
    {
        for(auto& client : _clients)
        {
            SocketMessage socketMessage;
            if(client.checkMessage(socketMessage))
            {
                if(socketMessage.messageType == Client::MESH_CONNECTION)
                {
                    Client::ConnectionMessage message;
                    memcpy(&message, &socketMessage.message[0], socketMessage.message.size());
                    
                    if(message.parentId == 0)
                    {
                        message.parentId = SyncBlink::getId();
                        memcpy(&socketMessage.message[0], &message, sizeof(message));
                        client.setStreamId(message.clientId);
                    }
                    #ifdef DEBUG_SOCKET
                    Serial.printf("[TCP SERVER] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                        message.clientId, message.ledCount,
                        message.parentId, message.majorVersion, message.minorVersion);
                    #endif
                }
                for (auto event : messageEvents.getEventHandlers())
                    event.second(socketMessage);
            }
        }
    }
}