#ifndef NODECONNECTOR_H
#define NODECONNECTOR_H

#include "node_rom.hpp"

#include <ESP8266WiFi.h>
#include <led/led.hpp>
#include <network/tcp/udp_discover.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/tcp/tcp_client.hpp>
#include <network/mesh/syncblink_mesh.hpp>

namespace SyncBlink
{
    class NodeConnector
    {
    public:
        NodeConnector(NodeRom& nodeRom, LED& led);

        void loop();
        bool connectToMesh();

        bool isConnected();
        uint32_t getMeshLedCount();
        uint32_t getMeshNodeCount();
        uint32_t getPrevLedCount();
        uint32_t getPrevNodeCount();

        TcpClient& getTcpClient();
        TcpServer& getTcpServer();

    private:
        void registerEvents();
        void onSocketClientConnectionChanged(bool connected);
        void onMeshUpdateReceived(Server::UpdateMessage message);

        IPAddress _serverIP;
        bool _connectedToMeshWiFi = false;

        uint32_t _meshLedCount = 0;
        uint32_t _meshNodeCount = 0;
        uint32_t _previousLedCount = 0;
        uint32_t _previousNodeCount = 0;

        NodeRom& _nodeRom;
        LED& _led;
        UdpDiscover _udpDiscover;        
        TcpServer _tcpServer;
        TcpClient _tcpClient;
        SyncBlinkMesh _mesh;
    };
}

#endif // NODECONNECTOR_H