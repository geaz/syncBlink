#ifndef NODECONTEXT_H
#define NODECONTEXT_H

#include <memory>
#include <vector>
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
            
            void onSocketClientConnectionChanged(bool connected);
            void onMeshUpdateReceived(Server::UpdateMessage message);
            void onAnalyzerResultReceived(AudioAnalyzerMessage message);
            void onSocketClientModReceived(std::string mod);
            void onFirmwareFlashReceived(std::vector<uint8_t> data, Server::MessageType messageType);
            void onSocketServerMessageReceived(Client::MessageType messageType, uint8_t* payload, size_t length);

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