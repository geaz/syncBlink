#include "node_wifi_module.hpp"
#include "core/network/get_id.hpp"
#include "core/message/messages/mesh_connection.hpp"

namespace SyncBlink
{
    NodeWifiModule::NodeWifiModule(Config& config, MessageBus& messageBus) : 
        _config(config), _messageBus(messageBus),
        _mesh(_config.Values["wifi_ssid"], _config.Values["wifi_pw"]),
        _tcpServer(_messageBus)
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshUpdate>(this);
    }

    NodeWifiModule::~NodeWifiModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
    }

    void NodeWifiModule::setup()
    { 
        _mesh.connectWifi();
        if(!_mesh.tryJoinMesh())
        {
            Serial.print("[WIFI] Not connected to mesh. Going to sleep and trying again later ...\n");
            ESP.deepSleep(30 * 1000000);
        }

        _tcpClient = std::make_shared<TcpClient>(_messageBus);
        _tcpClient->start(_mesh.getParentIp().toString(), 81);
        _tcpServer.start();

        Messages::MeshConnection msg;
        msg.nodeId = SyncBlink::getId();
        msg.isConnected = true;
        msg.nodeInfo = { false, _config.Values["is_analyzer"], true, _mesh.isConnectedToMeshWifi(), 
            0, _config.Values["led_count"], VERSIONMAJOR, VERSIONMINOR, _config.Values["name"] };

        _tcpClient->writeMessage(msg.toPackage());
    }

    void NodeWifiModule::loop()
    {   
        _tcpServer.loop();
        _tcpClient->loop();
    }

    void NodeWifiModule::onMsg(const Messages::MeshUpdate& msg)
    {
        Messages::MeshUpdate updatedMsg;
        updatedMsg.script = msg.script;
        updatedMsg.meshLedCount = msg.meshLedCount;
        updatedMsg.meshNodeCount = msg.meshLedCount;
        updatedMsg.routeLedCount = msg.routeLedCount + _config.Values["led_count"].as<uint32_t>();
        updatedMsg.routeNodeCount = msg.routeNodeCount + 1;

        _tcpClient->writeMessage(updatedMsg.toPackage());
    }
}