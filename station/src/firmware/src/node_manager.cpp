#include "node_manager.hpp"

#include <algorithm>

namespace SyncBlink
{
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

    uint32_t NodeManager::getTotalLedCount() const
    {
        return _totalLeds;
    }
    
    uint32_t NodeManager::getTotalNodeCount() const
    {
        return _totalNodes;
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