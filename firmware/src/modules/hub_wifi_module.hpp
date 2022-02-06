#ifndef HUBWIFIMODULE_H
#define HUBWIFIMODULE_H

#include "module.hpp"
#include "script_module.hpp"
#include "core/config/config.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/udp/udp_discover.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/message/messages/mesh_connection.hpp"

#include <map>

namespace SyncBlink
{
    class HubWifiModule : 
        public Module,
        public MessageHandler<Messages::MeshConnection>
    {
    public:
        HubWifiModule(Config& config, MessageBus& messageBus, ScriptModule& scriptModule);
        ~HubWifiModule();

        void setup() override;
        void loop();

        void onMsg(const Messages::MeshConnection& msg);

    private:
        void addNode(uint64_t nodeId, NodeInfo nodeInfo);
        void removeNode(uint64_t nodeId);
        void countLeds();

        Config& _config;
        MessageBus& _messageBus;
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