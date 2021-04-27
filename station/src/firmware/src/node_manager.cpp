#include "node_manager.hpp"

#include <algorithm>
#include <LittleFS.h>

namespace SyncBlink
{
    NodeManager::NodeManager(SocketServer& socketServer) : _socketServer(socketServer) { }

    void NodeManager::loopFlashing()
    {
        if(_flashing)
        {
            if(_flashPos == _activeFlash.size())
            {
                _activeFlash.close();
                _flashing = false;
                _socketServer.broadcast(0, 0, Server::FIRMWARE_FLASH_END);
            }
            else
            {
                size_t readSize = _activeFlash.size() - _flashPos > 1024 
                    ? 1024
                    : _activeFlash.size() - _flashPos;

                char buf[readSize];
                _activeFlash.readBytes(buf, readSize);
                _flashPos += readSize;

                _socketServer.broadcast(buf, readSize, Server::FIRMWARE_FLASH_DATA);
            }
        }
    }

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

    void NodeManager::flash(uint64_t clientId)
    {
        if(LittleFS.exists(FirmwarePath.c_str()))
        {
            _activeFlash = LittleFS.open(FirmwarePath.c_str(), "r");
            _flashPos = 0;
            _flashing = true;

            _socketServer.broadcast(0, 0, Server::FIRMWARE_FLASH_START);
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