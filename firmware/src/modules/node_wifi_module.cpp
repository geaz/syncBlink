#include "node_wifi_module.hpp"

#include "core/message/messages/mesh_connection.hpp"
#include "core/network/get_id.hpp"

namespace SyncBlink
{
    NodeWifiModule::NodeWifiModule(Config& config, MessageBus& messageBus)
        : _config(config), _messageBus(messageBus), _mesh(_config.Values["wifi_ssid"], _config.Values["wifi_pw"]), _tcpServer(_messageBus)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshConnection>(this);
        _meshUpdateHandleId = _messageBus.addMsgHandler<Messages::MeshUpdate>(this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(this);
    }

    NodeWifiModule::~NodeWifiModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_meshUpdateHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
    }

    void NodeWifiModule::setup()
    {
        _mesh.connectWifi();
        if (!_mesh.tryJoinMesh())
        {
            Serial.print("[WIFI] Not connected to mesh. Going to sleep and trying again later ...\n");
            ESP.deepSleep(SleepSeconds * 1000000);
        }

        _tcpClient = std::make_shared<TcpClient>(_messageBus);
        _tcpClient->start(_mesh.getParentIp().toString(), 81);
        _tcpServer.start();

        Messages::MeshConnection msg;
        msg.nodeId = SyncBlink::getId();
        msg.isConnected = true;
        msg.nodeInfo = {
            false,        _config.Values["is_analyzer"], true, _mesh.isConnectedToMeshWifi(), 0, _config.Values["led_count"], VERSIONMAJOR,
            VERSIONMINOR, _config.Values["name"]};

        _tcpClient->writeMessage(msg.toPackage());
    }

    void NodeWifiModule::loop()
    {
        _tcpServer.loop();
        _tcpClient->loop();

        if (!_tcpClient->isConnected())
        {
            Serial.print("Going to sleep ...\n");
            ESP.deepSleep(SleepSeconds * 1000000);
        }
    }

    void NodeWifiModule::onMsg(const Messages::MeshConnection& msg)
    {
        _tcpClient->writeMessage(msg.toPackage());
    }

    void NodeWifiModule::onMsg(const Messages::MeshUpdate& msg)
    {
        Messages::MeshUpdate updatedMsg;
        updatedMsg.script = msg.script;
        updatedMsg.meshLedCount = msg.meshLedCount;
        updatedMsg.meshNodeCount = msg.meshLedCount;
        updatedMsg.routeLedCount = msg.routeLedCount + _config.Values["led_count"].as<uint32_t>();
        updatedMsg.routeNodeCount = msg.routeNodeCount + 1;

        _tcpServer.broadcast(updatedMsg.toPackage());
    }

    void NodeWifiModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }

    void NodeWifiModule::onMsg(const Messages::ScriptChange& msg)
    {
        _tcpServer.broadcast(msg.toPackage());
    }
}