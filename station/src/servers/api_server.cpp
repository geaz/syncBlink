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
               /* case Api::PING_NODE:
                {
                    auto targetNodeId = message.as<uint64_t>();
                    _nodeManager.pingNode(targetNodeId);

                    _blinkTcpServer.broadcast(&nodeId, sizeof(nodeId), Server::PING_NODE);
                    break;
                }
                case Api::RENAME_NODE:
                {
                    auto renameMessage = message.as<Server::NodeRenameMessage>();
                    _nodeManager.renameNode(renameMessage.nodeId, renameMessage.nodeLabel);
                    break;
                }
                case Api::SET_ANALYZER:
                case Api::SET_LIGHTMODE:
                case Api::UPDATE_FIRMWARE: 

                These messages just have to get forwarded - maybe a FORWARDE API CALL? */


                case Api::GET_WIFI:
                case Api::GET_SCRIPT:
                case Api::LIST_SCRIPTS:
                case Api::GET_INFO:
                {
                    
                    break;
                }
                case Api::SET_WIFI:
                case Api::ADD_SCRIPT:
                case Api::UPDATE_SCRIPT:
                case Api::DELETE_SCRIPT:
                case Api::SET_ACTIVE_SCRIPT:
                    break;
            }
        }
    }
}