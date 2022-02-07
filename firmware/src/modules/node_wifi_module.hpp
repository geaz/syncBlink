#ifndef NODEWIFIMODULE_H
#define NODEWIFIMODULE_H

#include "module.hpp"
#include "core/config/config.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/tcp/tcp_client.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"
#include "core/message/messages/mesh_update.hpp"

namespace SyncBlink
{
    class NodeWifiModule : 
        public Module,        
        public MessageHandler<Messages::MeshUpdate>
    {
    public:
        NodeWifiModule(Config& config, MessageBus& messageBus);
        ~NodeWifiModule();
        
        void setup() override;
        void loop();

        void onMsg(const Messages::MeshUpdate& msg);

    private:
        Config& _config;
        MessageBus& _messageBus;
        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        std::shared_ptr<TcpClient> _tcpClient;

        uint32_t _meshHandleId = 0;
    };
}

#endif // NODEWIFIMODULE_H