#ifndef NODEWIFIMODULE_H
#define NODEWIFIMODULE_H

#include "core/config/config.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/node_command.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/network/tcp/tcp_client.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/script_list.hpp"
#include "core/mesh_info.hpp"
#include "module.hpp"

#include <led.hpp>

namespace SyncBlink
{
    static const uint8_t SleepSeconds = 30;

    class NodeWifiModule : public Module,
                           public MessageHandler<Messages::MeshUpdate>,
                           public MessageHandler<Messages::MeshConnection>,
                           public MessageHandler<Messages::AnalyzerUpdate>,
                           public MessageHandler<Messages::NodeCommand>,
                           public MessageHandler<Messages::RawBytes>
    {
    public:
        NodeWifiModule(Config& config, LED& led, MessageBus& messageBus, ScriptList& scriptList, MeshInfo& meshInfo);
        ~NodeWifiModule();

        void setup() override;
        void loop() override;

        void onMsg(const Messages::NodeCommand& msg);
        void onMsg(const Messages::MeshUpdate& msg);
        void onMsg(const Messages::MeshConnection& msg);
        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::RawBytes& msg);

    private:
        Config& _config;
        LED& _led;
        MessageBus& _messageBus;
        ScriptList& _scriptList;
        MeshInfo& _meshInfo;

        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        std::shared_ptr<TcpClient> _tcpClient;

        bool _isReceivingScript = false;
        Script _activeScriptReceive;

        uint32_t _rawHandleId = 0;
        uint32_t _meshHandleId = 0;
        uint32_t _nodeCommandHandleId = 0;
        uint32_t _meshUpdateHandleId = 0;
        uint32_t _analyzerHandleId = 0;
    };
}

#endif // NODEWIFIMODULE_H