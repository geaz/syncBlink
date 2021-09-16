#include "api_server.hpp"

namespace SyncBlink
{
    ApiServer::ApiServer(NodeManager& nodeManager) : _nodeManager(nodeManager)
    { }

    void ApiServer::loop()
    {
        Message message;
        if(Message::available(Serial, message))
        {
            switch (message.type)
            {
                case Api::GET_INFO:
                {
                    
                    break;
                }
                case Api::PING_NODE:
                {
                    uint64_t targetNodeId = 0;
                    memcpy(&targetNodeId, &message.body[0], message.body.size());
                    _nodeManager.pingNode(targetNodeId);
                    break;
                }
            }
        }
    }
}