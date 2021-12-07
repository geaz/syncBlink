#include "node_manager.hpp"

#include <algorithm>

namespace SyncBlink
{
    NodeManager::NodeManager(LED& led, TcpServer& tcpServer, StationWifi& stationWifi) : _led(led), _tcpServer(tcpServer), _stationWifi(stationWifi)
    {
        _conHandleId = _tcpServer.serverConnectionEvents
            .addEventHandler([this](Client::ConnectionMessage connectionMessage) { onMeshConnection(connectionMessage); });
        _disConHandleId = _tcpServer.serverDisconnectionEvents
            .addEventHandler([this](uint64_t nodeId) { onMeshDisconnection(nodeId); });
    }

    NodeManager::~NodeManager()
    {
        _tcpServer.serverConnectionEvents.removeEventHandler(_conHandleId);
        _tcpServer.serverDisconnectionEvents.removeEventHandler(_disConHandleId);
    }

    void NodeManager::setLightMode(bool lightMode)
    {
        _lightMode = lightMode;
        _tcpServer.broadcast(&lightMode, sizeof(lightMode), Server::LIGHT_MODE);
    }

    void NodeManager::setAnalyzer(uint64_t analyzerId)
    {
        _activeAnalyzer = analyzerId;
        _tcpServer.broadcast(&analyzerId, sizeof(analyzerId), Server::SOURCE_UPDATE);
    }

    void NodeManager::pingNode(uint64_t nodeId)
    {
        _tcpServer.broadcast(&nodeId, sizeof(nodeId), Server::PING_NODE);
    }

    void NodeManager::renameNode(uint64_t nodeId, const std::string& label)
    {
        Server::NodeRenameMessage message;
        message.nodeId = nodeId;
        memcpy(&message.nodeLabel[0], &label[0], MaxNodeLabelLength > label.size() ? MaxNodeLabelLength : label.size());

        _tcpServer.broadcast(&message, sizeof(message), Server::NODE_RENAME);

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

    void NodeManager::setWifi(uint64_t nodeId, bool meshWifi)
    {
        Server::WifiSetMessage message;
        message.nodeId = nodeId;

        memset(&message.ssid[0], 0, WifiRomSSIDLength);
        memset(&message.pw[0], 0, WifiRomPwLength);
        
        if(!meshWifi)
        {
            memcpy(&message.ssid[0], &_stationWifi.getSavedSSID()[0], _stationWifi.getSavedSSID().size());
            memcpy(&message.pw[0], &_stationWifi.getSavedPass()[0], _stationWifi.getSavedPass().size());
        }

        _tcpServer.broadcast(&message, sizeof(message), Server::SET_WIFI);
    }

    void NodeManager::onMeshConnection(Client::ConnectionMessage connectionMessage)
    {
        if(connectionMessage.isAnalyzer && !connectionMessage.isNode)
        {
            Serial.printf("[MESH] New Analyzer connected: %s\n", connectionMessage.nodeLabel);
        }
        else
        {
            Serial.printf("[MESH] New Node: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                connectionMessage.nodeId, connectionMessage.ledCount,
                connectionMessage.parentId, connectionMessage.majorVersion, connectionMessage.minorVersion);
        }

        _connectedNodes.push_back(connectionMessage);
        countInfos();

        Server::UpdateMessage updateMessage = { _activeAnalyzer, _led.getLedCount(), 1, _totalLeds, _totalNodes };
        _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
    }

    void NodeManager::onMeshDisconnection(uint64_t nodeId)
    {
        // CLEAR RECURSIVE!!!!!!
        _connectedNodes.erase(
            std::remove_if(_connectedNodes.begin(), _connectedNodes.end(),
                [nodeId](const Client::ConnectionMessage& m){ return m.nodeId == nodeId || m.parentId == nodeId; }),
            _connectedNodes.end());
        countInfos();

        Serial.printf("[MESH] Node disconnected: %12llx\n", nodeId);

        Server::UpdateMessage updateMessage = { _activeAnalyzer, _led.getLedCount(), 1, _totalLeds, _totalNodes };
        _tcpServer.broadcast(&updateMessage, sizeof(updateMessage), Server::MESH_UPDATE);
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
    
    bool NodeManager::getLightMode() const { return _lightMode; }
    uint64_t NodeManager::getActiveAnalyzer() const { return _activeAnalyzer; }
    uint32_t NodeManager::getTotalLedCount() const { return _totalLeds; }    
    uint32_t NodeManager::getTotalNodeCount() const { return _totalNodes; }
    std::vector<Client::ConnectionMessage> NodeManager::getConnectedNodes() const { return _connectedNodes; }
}