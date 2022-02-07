#include "hub_wifi_module.hpp"

namespace SyncBlink
{
    HubWifiModule::HubWifiModule(Config& config, MessageBus& messageBus, ScriptModule& scriptModule) : 
        _config(config), _messageBus(messageBus), _scriptModule(scriptModule), 
        _mesh(config.Values["wifi_ssid"], config.Values["wifi_pw"]),
        _tcpServer(messageBus)
    {        
        _meshConHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(this);
    }

    HubWifiModule::~HubWifiModule()
    {
        _messageBus.removeMsgHandler(_meshConHandleId);
    }

    void HubWifiModule::setup()
    { 
        _mesh.connectWifi();
        _mesh.startMesh();

        _tcpServer.start();
        _udpDiscover.start(true);
    }

    void HubWifiModule::loop()
    {
        _tcpServer.loop();
        _udpDiscover.loop();
    }
        
    void HubWifiModule::onMsg(const Messages::MeshConnection& msg)
    {
        if(msg.isConnected) 
        {
            addNode(msg.nodeId, msg.nodeInfo);
            if(msg.nodeInfo.isAnalyzer && !msg.nodeInfo.isNode)
            {
                Serial.printf("[WIFI] New Analyzer connected: %s\n", msg.nodeInfo.nodeLabel.c_str());
            }
            else
            {
                Serial.printf("[WIFI] New Client: %12llx - LEDs %i - Parent %12llx - Firmware Version: %i.%i\n",
                    msg.nodeId, msg.nodeInfo.ledCount, msg.nodeInfo.parentId, msg.nodeInfo.majorVersion, msg.nodeInfo.minorVersion);
            }
        }
        else
        {
            removeNode(msg.nodeId);
            Serial.printf("[WIFI] Node disconnected: %12llx\n", msg.nodeId);
        }

        countLeds();

        Messages::MeshUpdate updateMsg = {_scriptModule.getActiveScript(), _config.Values["led_count"], 1, _totalLeds, _totalNodes};
        _messageBus.trigger(updateMsg);
    }

    void HubWifiModule::addNode(uint64_t nodeId, NodeInfo nodeInfo)
    {
        _connectedNodes[nodeId] = nodeInfo;
    }

    void HubWifiModule::removeNode(uint64_t nodeId)
    {
        auto iter = _connectedNodes.begin();
        auto endIter = _connectedNodes.end();
        for(; iter != endIter; ) {
            if (iter->first == nodeId || iter->second.parentId == nodeId) {
                iter = _connectedNodes.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void HubWifiModule::countLeds()
    {
        _totalNodes = _totalLeds = 0;
        for(auto& node : _connectedNodes)
        {
            _totalNodes++;
            _totalLeds += node.second.ledCount;
        }
    }
}