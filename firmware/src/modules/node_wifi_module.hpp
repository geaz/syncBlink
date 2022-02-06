#ifndef NODEWIFIMODULE_H
#define NODEWIFIMODULE_H

#include "module.hpp"
#include "core/config/config.hpp"
#include "core/network/tcp/tcp_server.hpp"
#include "core/network/tcp/tcp_client.hpp"
#include "core/network/mesh/syncblink_mesh.hpp"

namespace SyncBlink
{
    class NodeWifiModule : public Module
    {
    public:
        NodeWifiModule(Config& config, MessageBus& messageBus);

        void setup() override;
        void loop();

    private:
        Config& _config;
        MessageBus& _messageBus;
        SyncBlinkMesh _mesh;
        TcpServer _tcpServer;
        std::shared_ptr<TcpClient> _tcpClient;
    };
}

#endif // NODEWIFIMODULE_H