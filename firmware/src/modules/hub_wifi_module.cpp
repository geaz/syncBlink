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
        if(msg.isConnected) addNode(msg.nodeId, msg.nodeInfo);
        else removeNode(msg.nodeId);

        countLeds();

        Script activeScript = _scriptModule.getActiveScript();
        Messages::MeshUpdate updateMsg = {activeScript, _config.Values["led_count"], 1, _totalLeds, _totalNodes};
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