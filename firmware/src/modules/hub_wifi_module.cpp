#include "hub_wifi_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    HubWifiModule::HubWifiModule(Config& config, MessageBus& messageBus, ScriptModule& scriptModule)
        : _config(config), _messageBus(messageBus), _scriptModule(scriptModule),
          _mesh(config.Values["wifi_ssid"], config.Values["wifi_pw"]), _tcpServer(messageBus)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(this);
        _nodeCommandHandleId = _messageBus.addMsgHandler<Messages::NodeCommand>(this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(this);
    }

    HubWifiModule::~HubWifiModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
        _messageBus.removeMsgHandler(_nodeCommandHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
    }

    void HubWifiModule::setup()
    {
        _mesh.connectWifi();
        _mesh.startMesh();

        _tcpServer.start();
    }

    void HubWifiModule::loop()
    {
        _tcpServer.loop();
    }

    void HubWifiModule::onMsg(const Messages::MeshConnection& msg)
    {
        if (msg.isConnected)
        {
            addNode(msg.nodeId, msg.nodeInfo);
            if (msg.nodeInfo.isAnalyzer && !msg.nodeInfo.isNode)
            {
                Serial.printf("[WIFI] New Analyzer connected: %s\n", msg.nodeInfo.nodeLabel.c_str());
            }
            else
            {
                Serial.printf("[WIFI] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n", msg.nodeId,
                              msg.nodeInfo.ledCount, msg.nodeInfo.parentId, msg.nodeInfo.majorVersion, msg.nodeInfo.minorVersion);
            }
        }
        else
        {
            removeNode(msg.nodeId);
            Serial.printf("[WIFI] Node disconnected: %12llx\n", msg.nodeId);
        }

        countLeds();

        Messages::MeshUpdate updateMsg = {_scriptModule.getActiveScript(), _config.Values["led_count"], 1, _totalLeds, _totalNodes};
        _tcpServer.broadcast(updateMsg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::ScriptChange& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::NodeCommand& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::addNode(uint64_t nodeId, NodeInfo nodeInfo)
    {
        _connectedNodes[nodeId] = nodeInfo;
    }

    void HubWifiModule::removeNode(uint64_t nodeId)
    {
        auto iter = _connectedNodes.begin();
        auto endIter = _connectedNodes.end();
        for (; iter != endIter;)
        {
            if (iter->first == nodeId || iter->second.parentId == nodeId)
            {
                iter = _connectedNodes.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    void HubWifiModule::countLeds()
    {
        _totalNodes = _totalLeds = 0;
        for (auto& node : _connectedNodes)
        {
            _totalNodes++;
            _totalLeds += node.second.ledCount;
        }
    }

    std::tuple<uint64_t, NodeInfo> HubWifiModule::getStationInfo() const
    {
        NodeInfo stationInfo = {
            true, true, false, true, 0, _config.Values["led_count"], VERSIONMAJOR, VERSIONMINOR, _config.Values["name"]};
        return std::make_tuple(SyncBlink::getId(), stationInfo);
    }

    std::map<uint64_t, NodeInfo> HubWifiModule::getConnectedNodes() const
    {
        return _connectedNodes;
    }
}