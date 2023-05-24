#ifndef HUBWIFIMODULE_H
#define HUBWIFIMODULE_H

#include "core/config/config.hpp"
#include "core/message/messages/analyzer_change.hpp"
#include "core/message/messages/mesh_connection.hpp"
#include "core/message/messages/script_load.hpp"
#include "core/message/messages/node_command.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/udp/udp_discover.hpp"
#include "core/script_list.hpp"
#include "core/mesh_info.hpp"
#include "module.hpp"

#include <map>
#include <tuple>

namespace SyncBlink
{
    class HubWifiModule : public Module,
                          public MessageHandler<Messages::MeshConnection>,
                          public MessageHandler<Messages::AnalyzerUpdate>,
                          public MessageHandler<Messages::AnalyzerChange>,
                          public MessageHandler<Messages::RawBytes>,
                          public MessageHandler<Messages::NodeCommand>
    {
    public:
        HubWifiModule(Config& config, MessageBus& messageBus, ScriptList& scriptList, MeshInfo& meshInfo);
        ~HubWifiModule();

        void setup() override;
        void loop() override;

        void onMsg(const Messages::MeshConnection& msg);
        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::AnalyzerChange& msg);
        void onMsg(const Messages::RawBytes& msg);
        void onMsg(const Messages::NodeCommand& msg);

    private:
        Config& _config;
        MessageBus& _messageBus;
        ScriptList& _scriptList;
        MeshInfo& _meshInfo;

        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        UdpDiscover _udpDiscover;

        uint32_t _meshHandleId = 0;
        uint32_t _analyzerHandleId = 0;
        uint32_t _analyzerChangeHandleId = 0;
        uint32_t _nodeCommandHandleId = 0;
        uint32_t _rawHandleId = 0;

        uint64_t _lastPing = 0;
    };
}

#endif // HUBWIFIMODULE_H