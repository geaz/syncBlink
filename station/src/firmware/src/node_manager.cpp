#include "node_manager.hpp"

#include <algorithm>

namespace SyncBlink
{
    NodeManager::NodeManager(SocketServer& socketServer) : _socketServer(socketServer) { }

    void NodeManager::addNode(Client::ConnectionMessage connectionMessage)
    {
        _connectedNodes.push_back(connectionMessage);
        countInfos();
    }

    void NodeManager::removeNode(uint64_t clientId)
    {
        _connectedNodes.erase(
            std::remove_if(_connectedNodes.begin(), _connectedNodes.end(),
                [clientId](const Client::ConnectionMessage& m){ return m.clientId == clientId || m.parentId == clientId; }),
            _connectedNodes.end());
        countInfos();
    }

    void NodeManager::renameNode(uint64_t clientId, const std::string& label)
    {
        Server::NodeRenameMessage message;
        message.clientId = clientId;
        memcpy(&message.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());

        _socketServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);

        // Also save the info into the known nodes vector
        for(auto& node : _connectedNodes)
        {
            if(node.clientId == clientId)
            {
                memcpy(&node.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());
                break;
            }
        }
    }

    uint32_t NodeManager::getTotalLedCount() const
    {
        return _totalLeds;
    }
    
    uint32_t NodeManager::getTotalNodeCount() const
    {
        return _totalNodes;
    }

    std::vector<Client::ConnectionMessage> NodeManager::getConnectedNodes() const
    {
        return _connectedNodes;
    }

    void NodeManager::countInfos()
    {        
        _totalNodes = _totalLeds = 0;
        for(auto& node : _connectedNodes)
        {
            _totalNodes++;
            _totalLeds += node.ledCount;
        }
    }
}