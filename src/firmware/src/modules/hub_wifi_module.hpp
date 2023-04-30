#ifndef HUBWIFIMODULE_H
#define HUBWIFIMODULE_H

#include "core/config/config.hpp"
#include "core/message/messages/analyzer_change.hpp"
#include "core/message/messages/mesh_connection.hpp"
#include "core/message/messages/node_command.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/udp/udp_discover.hpp"
#include "module.hpp"
#include "script_module.hpp"

#include <map>
#include <tuple>

namespace SyncBlink
{
    class HubWifiModule : public Module,
                          public MessageHandler<Messages::MeshConnection>,
                          public MessageHandler<Messages::AnalyzerUpdate>,
                          public MessageHandler<Messages::AnalyzerChange>,
                          public MessageHandler<Messages::NodeCommand>,
                          public MessageHandler<Messages::ScriptChange>
    {
    public:
        HubWifiModule(Config& config, MessageBus& messageBus, ScriptModule& scriptModule);
        ~HubWifiModule();

        void setup() override;
        void loop();

        void onMsg(const Messages::MeshConnection& msg);
        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::AnalyzerChange& msg);
        void onMsg(const Messages::ScriptChange& msg);
        void onMsg(const Messages::NodeCommand& msg);

        std::tuple<uint64_t, NodeInfo> getStationInfo() const;
        std::map<uint64_t, NodeInfo> getConnectedNodes() const;

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

        uint32_t _meshHandleId = 0;
        uint32_t _analyzerHandleId = 0;
        uint32_t _analyzerChangeHandleId = 0;
        uint32_t _nodeCommandHandleId = 0;
        uint32_t _scriptHandleId = 0;

        uint32_t _totalLeds;
        uint32_t _totalNodes;

        std::map<uint64_t, NodeInfo> _connectedNodes;
    };
}

#endif // HUBWIFIMODULE_H