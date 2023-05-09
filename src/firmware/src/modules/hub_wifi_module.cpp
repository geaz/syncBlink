#include "hub_wifi_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    HubWifiModule::HubWifiModule(Config& config, MessageBus& messageBus, ScriptModule& scriptModule)
        : _config(config), _messageBus(messageBus), _scriptModule(scriptModule),
          _mesh(config.Values[F("wifi_ssid")], config.Values[F("wifi_pw")]), _tcpServer(messageBus)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(MessageType::MeshConnection, this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(MessageType::AnalyzerUpdate, this);
        _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(MessageType::AnalyzerChange, this);
        _nodeCommandHandleId = _messageBus.addMsgHandler<Messages::NodeCommand>(MessageType::NodeCommand, this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(MessageType::ScriptChange, this);
    }

    HubWifiModule::~HubWifiModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
        _messageBus.removeMsgHandler(_analyzerChangeHandleId);
        _messageBus.removeMsgHandler(_nodeCommandHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
    }

    void HubWifiModule::setup()
    {
        _mesh.connectWifi();
        _mesh.startMesh();
        _tcpServer.start();
        _udpDiscover.start(true);

        // Set the hub as the analyzer on start
        _messageBus.trigger(Messages::AnalyzerChange{SyncBlink::getId()});
        // Publish active script on start
        _messageBus.trigger(Messages::ScriptChange{_scriptModule.getActiveScript().Name});
    }

    void HubWifiModule::loop()
    {
        _tcpServer.loop();
        _udpDiscover.loop();
    }

    void HubWifiModule::onMsg(const Messages::MeshConnection& msg)
    {
        if (msg.isConnected)
        {
            addNode(msg.nodeId, msg.nodeInfo);
            if (msg.nodeInfo.isAnalyzer && !msg.nodeInfo.isNode)
            {
                Serial.printf_P(PSTR("[HUB] New Analyzer connected: %s\n"), msg.nodeInfo.nodeLabel.c_str());
            }
            else
            {
                Serial.printf_P(PSTR("[HUB] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n"), msg.nodeId,
                                msg.nodeInfo.ledCount, msg.nodeInfo.parentId, msg.nodeInfo.majorVersion, msg.nodeInfo.minorVersion);
            }
        }
        else
        {
            removeNode(msg.nodeId);
            Serial.printf_P(PSTR("[HUB] Node disconnected: %12llx\n"), msg.nodeId);
        }

        countLeds();
        Messages::MeshUpdate updateMsg = {_config.Values[F("led_count")], 1, _totalLeds, _totalNodes};
        _tcpServer.broadcast(updateMsg.toPackage());

        if (msg.isConnected)
        {
            sendScriptUpdate(msg.nodeId);
            Messages::ScriptChange scriptChange = {_scriptModule.getActiveScript().Name};
            _tcpServer.broadcast(scriptChange.toPackage());
        }
    }

    void HubWifiModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::AnalyzerChange& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::ScriptChange& msg)
    {
        sendScriptUpdate();
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::NodeCommand& msg)
    {
        if (msg.commandType == Messages::NodeCommandType::WifiChange)
        {
            removeNode(msg.recipientId);
            Serial.printf_P(PSTR("[HUB] Removing Node due to disconnecting command: %12llx\n"), msg.recipientId);
        }
        else if (msg.commandType == Messages::NodeCommandType::Rename)
        {
            // To avoid the need to reconnect the node to update the node label in memory through the mesh connection message,
            // the label just gets updated in memory directly.
            _connectedNodes[msg.recipientId].nodeLabel = msg.commandInfo.stringInfo1;
        }
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::sendScriptUpdate(uint64_t nodeId)
    {
        Messages::NodeCommand msg;
        msg.recipientId = nodeId;
        msg.commandInfo.stringInfo1 = _scriptModule.getActiveScript().Name;
        msg.commandType = Messages::NodeCommandType::ScriptUpdate;

        _tcpServer.broadcast(msg.toPackage());
        _tcpServer.broadcast(_scriptModule.getActiveScript().getBytecodeFile());

        msg.commandType = Messages::NodeCommandType::ScriptUpdated;
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
            true, true, false, true, 0, _config.Values[F("led_count")], VERSIONMAJOR, VERSIONMINOR, _config.Values[F("name")]};
        return std::make_tuple(SyncBlink::getId(), stationInfo);
    }

    const std::map<uint64_t, NodeInfo>& HubWifiModule::getConnectedNodes() const
    {
        return _connectedNodes;
    }
}