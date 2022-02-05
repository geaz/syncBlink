#include "node_wifi_module.hpp"
#include "core/network/get_id.hpp"
#include "core/event/event_types.hpp"
#include "core/event/events/mesh_connection_event.hpp"

namespace SyncBlink
{
    NodeWifiModule::NodeWifiModule(Config& config, EventBus& eventBus) : 
        _config(config), _eventBus(eventBus),
        _mesh(_config.Values["wifi_ssid"], _config.Values["wifi_pw"]),
        _tcpServer(_eventBus)
    { }

    void NodeWifiModule::setup()
    { 
        _mesh.connectWifi();
        if(!_mesh.tryJoinMesh())
        {
            Serial.print("[WIFI] Not connected to mesh. Going to sleep and trying again later ...\n");
            ESP.deepSleep(30 * 1000000);
        }

        _tcpClient = std::make_shared<TcpClient>(_eventBus);
        _tcpClient->start(_mesh.getParentIp().toString(), 81);
        _tcpServer.start();

        bool isAnalyzer = _config.Values["is_analyzer"] == "true";
        uint16_t ledCount = _config.Values["led_count"];
        std::string nodeLabel = _config.Values["name"];

        Events::MeshConnectionEvent event = { SyncBlink::getId(), true, { false,  isAnalyzer, true, _mesh.isConnectedToMeshWifi(),  0, ledCount, VERSIONMAJOR, VERSIONMINOR, nodeLabel } };
        _tcpClient->sendMessage(&event, sizeof(event), EventType::MeshConnectionEvent);
    }

    void NodeWifiModule::loop()
    {   
        _tcpServer.loop();
        _tcpClient->loop();
    }
}