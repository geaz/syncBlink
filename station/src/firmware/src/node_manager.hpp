#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <vector>
#include <led/led.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/mesh/syncblink_mesh.hpp>
#include <network/tcp/messages/client_messages.hpp>
#include <network/tcp/messages/server_messages.hpp>

namespace SyncBlink
{
    const std::string FirmwarePath = "/firmware.bin";

    class NodeManager
    {
        public:
            NodeManager(LED& led, TcpServer& blinkTcpServer);
            ~NodeManager();

            void setLightMode(bool lightMode);
            void setAnalyzer(uint64_t analyzerId);
            void pingNode(uint64_t nodeId);
            void renameNode(uint64_t nodeId, const std::string& label);

            bool getLightMode() const;
            uint64_t getActiveAnalyzer() const;
            uint32_t getTotalLedCount() const;
            uint32_t getTotalNodeCount() const;
            std::vector<Client::ConnectionMessage> getConnectedNodes() const;

        private:
            void onMeshConnection(Client::ConnectionMessage connectionMessage);
            void onMeshDisconnection(uint64_t nodeId);
            void countInfos();
            
            LED& _led;
            TcpServer& _tcpServer;

            uint32_t _conHandleId;
            uint32_t _disConHandleId;

            uint32_t _totalLeds;
            uint32_t _totalNodes;
            bool _lightMode = false;
            uint64_t _activeAnalyzer = SyncBlink::getId();

            std::vector<Client::ConnectionMessage> _connectedNodes = {{ true, true, false, SyncBlink::getId(), 0, LED_COUNT, VERSIONMAJOR, VERSIONMINOR, { 'S', 't', 'a', 't', 'i', 'o', 'n' } }};
    };
}

#endif // NODEMANAGER_H