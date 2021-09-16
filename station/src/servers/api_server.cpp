#include "api_server.hpp"

namespace SyncBlink
{
    ApiServer::ApiServer(TcpServer& blinkServer) : _blinkServer(blinkServer)
    {
        
    }

    void ApiServer::start()
    {
        
    }

    void ApiServer::loop()
    {
        
    }

    void ApiServer::onSocketServerCommandReceived(Message message)
    {
        switch (message.type)
        {
            case Api::PING_NODE:
            {
                uint64_t targetNodeId = 0;
                memcpy(&targetNodeId, &message.body[0], message.body.size());
                _blinkServer.broadcast(&targetNodeId, sizeof(targetNodeId), Server::PING_NODE);
                break;
            }
        }
    }
}