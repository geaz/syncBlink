#include "hub_wifi_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    HubWifiModule::HubWifiModule(Config& config, MessageBus& messageBus, ScriptList& scriptList, MeshInfo& meshInfo)
        : _config(config), _messageBus(messageBus), _scriptList(scriptList), _meshInfo(meshInfo),
          _mesh(config.Values[F("wifi_ssid")], config.Values[F("wifi_pw")]), _tcpServer(messageBus)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(MessageType::MeshConnection, this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(MessageType::AnalyzerUpdate, this);
        _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(MessageType::AnalyzerChange, this);
        _nodeCommandHandleId = _messageBus.addMsgHandler<Messages::NodeCommand>(MessageType::NodeCommand, this);
        _rawHandleId = _messageBus.addMsgHandler<Messages::RawBytes>(MessageType::RawBytes, this);
    }

    HubWifiModule::~HubWifiModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
        _messageBus.removeMsgHandler(_analyzerChangeHandleId);
        _messageBus.removeMsgHandler(_nodeCommandHandleId);
        _messageBus.removeMsgHandler(_rawHandleId);
    }

    void HubWifiModule::setup()
    {
        _mesh.connectWifi();
        _mesh.startMesh();
        _tcpServer.start();
        _udpDiscover.start(true);

        // Set the hub as the analyzer on start
        _messageBus.trigger(Messages::AnalyzerChange{SyncBlink::getId()});
        // Publish active script on start only on hub - thats why it is not done in the script module
        _messageBus.trigger(Messages::ScriptLoad{_meshInfo.getActiveScript().Name});
    }

    void HubWifiModule::loop()
    {
        // Ping nodes every five seconds to recognize any disconnects
        if (millis() - _lastPing > 5000)
        {
            SyncBlink::Messages::NodeCommand msg;
            msg.commandType = Messages::NodeCommandType::Ping;
            _tcpServer.broadcast(msg.toPackage());
            _lastPing = millis();
        }
        
        _tcpServer.loop();
        _udpDiscover.loop();
    }

    void HubWifiModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::AnalyzerChange& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::RawBytes& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void HubWifiModule::onMsg(const Messages::NodeCommand& msg)
    {
        _tcpServer.broadcast(msg.toPackage());

        if (msg.commandType == Messages::NodeCommandType::WifiChange)
        {
            _meshInfo.removeNode(msg.recipientId);
            Serial.printf_P(PSTR("[HUB] Removing Node due to disconnecting command: %12llx\n"), msg.recipientId);
        }
        else if (msg.commandType == Messages::NodeCommandType::Rename)
        {
            // To avoid the need to reconnect the node to update the node label in memory through the mesh connection message,
            // the label just gets updated in memory directly.
            _meshInfo.getConnectedNodes()[msg.recipientId].nodeLabel = msg.commandInfo.stringInfo1;
        }
    }

    void HubWifiModule::onMsg(const Messages::MeshConnection& msg)
    {
        if (msg.isConnected)
        {
            _meshInfo.addNode(msg.nodeId, msg.nodeInfo);
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
            _meshInfo.removeNode(msg.nodeId);
            Serial.printf_P(PSTR("[HUB] Node disconnected: %12llx\n"), msg.nodeId);

            if(_meshInfo.getActiveAnalyzer() == msg.nodeId)
            {
                Serial.printf_P(PSTR("[HUB] Active analyzer disconnected. Falling back to internal analyzer..."));
                _messageBus.trigger(Messages::AnalyzerChange{SyncBlink::getId()});
            }
        }

        std::tuple<size_t, size_t> totalNodesAndLeds = _meshInfo.getTotalNodesAndLeds();
        Messages::MeshUpdate updateMsg = {_config.Values[F("led_count")], 1, std::get<1>(totalNodesAndLeds), std::get<0>(totalNodesAndLeds)};
        _tcpServer.broadcast(updateMsg.toPackage());
    }
}