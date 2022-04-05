#ifndef NODEWIFIMODULE_H
#define NODEWIFIMODULE_H

#include "core/config/config.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/network/tcp/tcp_client.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "module.hpp"

namespace SyncBlink
{
    static const uint8_t SleepSeconds = 30;

    class NodeWifiModule : public Module,
                           public MessageHandler<Messages::MeshUpdate>,
                           public MessageHandler<Messages::MeshConnection>,
                           public MessageHandler<Messages::AnalyzerUpdate>,
                           public MessageHandler<Messages::ScriptChange>
    {
    public:
        NodeWifiModule(Config& config, MessageBus& messageBus);
        ~NodeWifiModule();

        void setup() override;
        void loop();

        void onMsg(const Messages::MeshUpdate& msg);
        void onMsg(const Messages::MeshConnection& msg);
        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::ScriptChange& msg);

    private:
        Config& _config;
        MessageBus& _messageBus;
        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        std::shared_ptr<TcpClient> _tcpClient;

        uint32_t _meshHandleId = 0;
        uint32_t _meshUpdateHandleId = 0;
        uint32_t _analyzerHandleId = 0;
        uint32_t _scriptHandleId = 0;
    };
}

#endif // NODEWIFIMODULE_H