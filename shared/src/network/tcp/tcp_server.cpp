#include "tcp_server.hpp"
#include "network/mesh/syncblink_mesh.hpp"

namespace SyncBlink
{
    void TcpServer::start()
    {
        _server.begin();
    }

    void TcpServer::loop()
    {
        clearClients();
        checkNewClients();
        handleIncomingMessages();
    }

    void TcpServer::broadcast(void* message, uint32_t messageSize, Server::MessageType messageType)
    {
        auto tcpMessage = TcpStream::serializeMessage(message, messageSize, messageType);
        #ifdef DEBUG_TCPSTREAM
        if(_clients.size() > 0) Serial.printf("[TCP SERVER] Writing message - Type: %i, Size: %i\n", messageType, tcpMessage.size());
        #endif
        for(auto& client : _clients)
        {
            client.writeMessage(tcpMessage);
        }
    }

    uint32_t TcpServer::getClientsCount()
    {
        return _clients.size();
    }

    void TcpServer::clearClients()
    {
        for(auto iter = _clients.begin(); iter != _clients.end();)
        {
            if(!iter->isConnected() || iter->isTimeout())
            {
                #ifdef DEBUG_TCP
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

    void TcpServer::checkNewClients()
    {
        if(_server.hasClient())
        {
            _clients.push_back(TcpStream(_server.available()));
        }
    }

    void TcpServer::handleIncomingMessages()
    {
        for(auto& client : _clients)
        {
            TcpMessage tcpMessage;
            if(client.checkMessage(tcpMessage))
            {
                if(tcpMessage.messageType == Client::MESH_CONNECTION)
                {
                    Client::ConnectionMessage message;
                    memcpy(&message, &tcpMessage.message[0], tcpMessage.message.size());
                    
                    if(message.parentId == 0)
                    {
                        message.parentId = SyncBlink::getId();
                        memcpy(&tcpMessage.message[0], &message, sizeof(message));
                        client.setStreamId(message.clientId);
                    }

                    // check for lost/timeout connections before
                    // informing about a new connection
                    // It could be a reconnection of a node which was previously connected and 
                    // no timeout was detected before...
                    broadcast(0, 0, Server::PING);
                    client.flush();
                    clearClients();
                    
                    #ifdef DEBUG_TCP
                    Serial.printf("[TCP SERVER] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                        message.clientId, message.ledCount,
                        message.parentId, message.majorVersion, message.minorVersion);
                    #endif
                }
                for (auto event : messageEvents.getEventHandlers())
                    event.second(tcpMessage);
            }
        }
    }
}