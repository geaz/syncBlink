#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <network/websocket/messages/client_messages.hpp>
#include <vector>

namespace SyncBlink
{
    class NodeManager
    {
        public:
            void addNode(Client::ConnectionMessage connectionMessage);
            void removeNode(uint64_t clientId);

            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void countInfos();
            
            uint32_t _totalLeds;
            uint32_t _totalNodes;

            std::vector<Client::ConnectionMessage> _connectedNodes;
    };
}

#endif // NODEMANAGER_H