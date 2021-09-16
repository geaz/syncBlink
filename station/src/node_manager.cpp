#include "node_manager.hpp"

#include <algorithm>

namespace SyncBlink
{
    NodeManager::NodeManager(TcpServer& blinkTcpServer) : _blinkTcpServer(blinkTcpServer) { }

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

    void NodeManager::setLightMode(bool lightMode)
    {
        _lightMode = lightMode;
        _blinkTcpServer.broadcast(&lightMode, sizeof(lightMode), Server::LIGHT_MODE);
    }

    void NodeManager::setAnalyzer(uint64_t analyzerId)
    {
        _activeAnalyzer = analyzerId;
        _blinkTcpServer.broadcast(&analyzerId, sizeof(analyzerId), Server::SOURCE_UPDATE);
    }

    void NodeManager::pingNode(uint64_t nodeId)
    {
        _blinkTcpServer.broadcast(&nodeId, sizeof(nodeId), Server::PING_NODE);
    }

    void NodeManager::renameNode(uint64_t nodeId, const std::string& label)
    {
        Server::NodeRenameMessage message;
        message.nodeId = nodeId;
        memcpy(&message.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());

        _blinkTcpServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);

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

    bool NodeManager::getLightMode() const
    {
        return _lightMode;
    }

    uint64_t NodeManager::getActiveAnalyzer() const
    {
        return _activeAnalyzer;
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