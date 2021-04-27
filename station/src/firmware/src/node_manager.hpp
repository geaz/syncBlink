#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <network/websocket/socket_server.hpp>
#include <network/websocket/messages/client_messages.hpp>
#include <network/websocket/messages/server_messages.hpp>
#include <vector>

namespace SyncBlink
{
    const std::string FirmwarePath = "/firmware.bin";

    class NodeManager
    {
        public:
            NodeManager(SocketServer& socketServer);

            void loopFlashing();
            void addNode(Client::ConnectionMessage connectionMessage);
            void removeNode(uint64_t clientId);

            void flash(uint64_t clientId);

            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void countInfos();
            
            uint32_t _totalLeds;
            uint32_t _totalNodes;

            File _activeFlash;
            size_t _flashPos;
            bool _flashing;

            SocketServer& _socketServer;
            std::vector<Client::ConnectionMessage> _connectedNodes;
    };
}

#endif // NODEMANAGER_H