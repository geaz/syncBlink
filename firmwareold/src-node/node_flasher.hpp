#ifndef NODEFLASHER_H
#define NODEFLASHER_H

#include <led/led.hpp>
#include <network/mesh/syncblink_mesh.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/tcp/tcp_client.hpp>

#include <vector>
#include <stdint.h>

namespace SyncBlink
{
    class NodeFlasher
    {
    public:
        NodeFlasher(TcpClient& tcpClient, TcpServer& tcpServer, LED& led);

    private:
        void onFirmwareFlashReceived(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType, TcpServer& tcpServer, LED& led);

        bool _flashActive = false;
    };
}

#endif // NODEFLASHER_H