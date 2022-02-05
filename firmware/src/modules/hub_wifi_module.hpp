#ifndef HUBWIFIMODULE_H
#define HUBWIFIMODULE_H

#include "module.hpp"
#include "script_module.hpp"
#include "core/config/config.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/udp/udp_discover.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/event/events/mesh_connection_event.hpp"

#include <map>

namespace SyncBlink
{
    class HubWifiModule : 
        public Module,
        public EventHandler<Events::MeshConnectionEvent>
    {
    public:
        HubWifiModule(Config& config, EventBus& eventBus, ScriptModule& scriptModule);
        ~HubWifiModule();

        void setup() override;
        void loop();

        void onEvent(const Events::MeshConnectionEvent& event);

    private:
        void addNode(uint64_t nodeId, NodeInfo nodeInfo);
        void removeNode(uint64_t nodeId);
        void countLeds();

        Config& _config;
        EventBus& _eventBus;
        ScriptModule& _scriptModule;

        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        UdpDiscover _udpDiscover;

        uint32_t _meshConHandleId;
        uint32_t _totalLeds;
        uint32_t _totalNodes;
        uint64_t _activeAnalyzer = 0;
        std::map<uint64_t, NodeInfo> _connectedNodes;
    };
}

#endif // HUBWIFIMODULE_H