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
            void removeNode(uint64_t nodeId);

            void setAnalyzer(uint64_t analyzerId);
            void pingNode(uint64_t nodeId);
            void renameNode(uint64_t nodeId, const std::string& label);

            uint64_t getActiveAnalyzer() const;
            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void countInfos();
            
            uint32_t _totalLeds;
            uint32_t _totalNodes;
            uint64_t _activeAnalyzer = SyncBlink::getId();

            TcpServer& _socketServer;
            std::vector<Client::ConnectionMessage> _connectedNodes = {{ true, true, false, SyncBlink::getId(), 0, LED_COUNT, VERSIONMAJOR, VERSIONMINOR, { 'S', 't', 'a', 't', 'i', 'o', 'n' } }};
    };
}

#endif // NODEMANAGER_H