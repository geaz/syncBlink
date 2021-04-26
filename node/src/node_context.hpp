#ifndef NODECONTEXT_H
#define NODECONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <blinkscript/blink_script.hpp>
#include <network/mesh/syncblink_mesh.hpp>
#include <network/websocket/socket_server.hpp>
#include <network/websocket/socket_client.hpp>

namespace SyncBlink
{
    static const uint8_t SleepSeconds = 60;

    class NodeContext
    {
        public:
            void setup();
            void loop();

        private:
            uint32_t readLedCount();

            void checkNewMod();
            void onSocketClientMessageReceived(Server::Message message);
            void onSocketClientModReceived(std::string mod);
            void onSocketServerMeshConnection();
            void onSocketServerMessageReceived(Client::Message message);

            LED _led;
            SyncBlinkMesh _mesh;
            SocketServer _socketServer;
            SocketClient _socketClient;

            uint32_t _nodeLedCount = 0;
            uint32_t _meshLedCount = 0;
            uint32_t _meshNodeCount = 0;
            uint32_t _previousLedCount = 0;
            uint32_t _previousNodeCount = 0;

            bool _newMod = false;
            std::string _currentMod;
            uint64_t _lastUpdate = millis();
            std::unique_ptr<BlinkScript> _blinkScript = nullptr;
    };
}

#endif // NODECONTEXT_H