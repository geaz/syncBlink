#ifndef NODECONTEXT_H
#define NODECONTEXT_H

#include <memory>
#include <vector>
#include <led/led.hpp>
#include <blinkscript/blink_script.hpp>
#include <audio/frequency_analyzer.hpp>
#include <network/mesh/syncblink_mesh.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/tcp/tcp_client.hpp>

namespace SyncBlink
{
    static const uint8_t SleepSeconds = 30;

    class NodeContext
    {
        public:
            void setup();
            void loop();

        private:
            void readNodeInfo();
            void checkNewScript();

            #ifdef MODE_PIN
            void checkModeButton();
            bool _lastButtonVal = false;
            unsigned long _lastButtonUpdate = 0;
            #endif

            #ifdef IS_ANALYZER
            void runAnalyzer();
            bool _isAnalyzer = true;
            uint64_t _lastLedUpdate = millis();
            FrequencyAnalyzer _frequencyAnalyzer;
            #else
            bool _isAnalyzer = false;
            #endif 

            void onSocketClientConnectionChanged(bool connected);
            void onMeshUpdateReceived(Server::UpdateMessage message);
            void onSocketClientScriptReceived(std::string script);
            void onAnalyzerResultReceived(AudioAnalyzerMessage message);
            void onNodeRenameReceived(Server::NodeRenameMessage message);
            void onFirmwareFlashReceived(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType);

            void onSocketServerMessageReceived(TcpMessage message);

            LED _led;
            SyncBlinkMesh _mesh;
            TcpServer _tcpServer;
            TcpClient _tcpClient;

            std::string _nodeLabel;

            uint32_t _nodeLedCount = 0;
            uint32_t _meshLedCount = 0;
            uint32_t _meshNodeCount = 0;
            uint32_t _previousLedCount = 0;
            uint32_t _previousNodeCount = 0;

            bool _newScript = false;
            bool _lightMode = false;
            bool _flashActive = false;
            uint64_t _activeAnalyzer = 0;

            std::string _currentScript;

            long _lastUpdate = millis();
            std::unique_ptr<BlinkScript> _blinkScript = nullptr;
    };
}

#endif // NODECONTEXT_H