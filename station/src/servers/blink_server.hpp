#ifndef BLINKSERVER_H
#define BLINKSERVER_H

#include "node_manager.hpp"
#include <led/led.hpp>
#include <tcp/tcp_server.hpp>

namespace SyncBlink
{
    class BlinkServer
    {
        public:
            BlinkServer(LED& led, NodeManager& nodeManager);
            void start();
            void loop();

            TcpServer& getTcpServer();

        private:
            void onMeshDisconnection(uint64_t clientId);
            void onSocketServerCommandReceived(Message message);

            LED& _led;
            NodeManager& _nodeManager;
            TcpServer _tcpServer = TcpServer(81);
    };
}

#endif // BLINKSERVER_H