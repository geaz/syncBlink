#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <network/mesh/syncblink_mesh.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/tcp/messages/client_messages.hpp>
#include <network/tcp/messages/server_messages.hpp>
#include <vector>

namespace SyncBlink
{
    const std::string FirmwarePath = "/firmware.bin";

    class NodeManager
    {
        public:
            NodeManager(TcpServer& tcpServer);

            void addNode(Client::ConnectionMessage connectionMessage);
            void removeNode(uint64_t clientId);
            void renameNode(uint64_t clientId, const std::string& label);

            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void countInfos();
            
            uint32_t _totalLeds;
            uint32_t _totalNodes;

            TcpServer& _socketServer;
            std::vector<Client::ConnectionMessage> _connectedNodes = {{ SyncBlink::getId(), 0, LED_COUNT, 0 }};
    };
}

#endif // NODEMANAGER_H