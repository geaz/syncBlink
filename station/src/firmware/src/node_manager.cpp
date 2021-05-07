#include "node_manager.hpp"

#include <algorithm>

namespace SyncBlink
{
    NodeManager::NodeManager(TcpServer& tcpServer) : _socketServer(tcpServer) { }

    void NodeManager::addNode(Client::ConnectionMessage connectionMessage)
    {
        _connectedNodes.push_back(connectionMessage);
        countInfos();
    }

    void NodeManager::removeNode(uint64_t nodeId)
    {
        _connectedNodes.erase(
            std::remove_if(_connectedNodes.begin(), _connectedNodes.end(),
                [nodeId](const Client::ConnectionMessage& m){ return m.nodeId == nodeId || m.parentId == nodeId; }),
            _connectedNodes.end());
        countInfos();
    }

    void NodeManager::setSource(uint64_t nodeId)
    {
        _activeSource = nodeId;
        _socketServer.broadcast(&nodeId, sizeof(nodeId), Server::SOURCE_UPDATE);
    }

    void NodeManager::pingNode(uint64_t nodeId)
    {
        _socketServer.broadcast(&nodeId, sizeof(nodeId), Server::PING);
    }

    void NodeManager::renameNode(uint64_t nodeId, const std::string& label)
    {
        Server::NodeRenameMessage message;
        message.nodeId = nodeId;
        memcpy(&message.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());

        _socketServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);

        // Also save the info into the known nodes vector
        for(auto& node : _connectedNodes)
        {
            if(node.nodeId == nodeId)
            {
                memcpy(&node.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());
                break;
            }
        }
    }

    uint64_t NodeManager::getActiveSource() const
    {
        return _activeSource;
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