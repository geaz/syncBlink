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
            void registerEvents();
            void checkNewScript();
            bool tryJoinMesh();
            void checkModeButton();
            void checkRunAnalyzer();
            void runStandaloneAnalyzer();

            void onSocketClientConnectionChanged(bool connected);
            void onMeshUpdateReceived(Server::UpdateMessage message);
            void onSocketClientScriptReceived(std::string script);
            void onAnalyzerResultReceived(AudioAnalyzerMessage message);
            void onNodeRenameReceived(Server::NodeRenameMessage message);
            void onFirmwareFlashReceived(std::vector<uint8_t> data, uint64_t targetNodeId, Server::MessageType messageType);

            void onSocketServerMessageReceived(TcpMessage message);

            LED _led;
            SyncBlinkMesh _mesh;
            TcpClient _tcpClient;
            TcpServer _tcpServer = TcpServer(81);
            FrequencyAnalyzer _frequencyAnalyzer;

            std::string _nodeLabel;

            bool _newScript = false;
            bool _lightMode = false;
            bool _flashActive = false;

            #ifdef IS_STANDALONE
            bool _isStandalone = true;
            #else
            bool _isStandalone = false;
            #endif

            bool _runStandalone = false;
            uint64_t _lastLedUpdate = millis();

            #ifdef MODE_PIN
            bool _hasModeButton = true;
            uint8_t _modePin = MODE_PIN;
            #else
            bool _hasModeButton = false;
            uint8_t _modePin = 0;
            #endif

            bool _lastButtonVal = false;
            unsigned long _lastButtonUpdate = 0;

            uint32_t _nodeLedCount = 0;
            uint32_t _meshLedCount = 0;
            uint32_t _meshNodeCount = 0;
            uint32_t _previousLedCount = 0;
            uint32_t _previousNodeCount = 0;

            uint64_t _activeAnalyzer = 0;

            std::string _currentScript;
            std::string _standaloneScript = "let h=0\nlet s=0\nlet v=0\nlet colors = []\n\nlet update = fun(delta) {\n\tif(vol == 0 || freq == 0 || (vol < lVol * 1.05 && v > 0.25)){\n        if(v > 0.025){ v = v - 0.025 }\n        else{ v = 0 }\n    } else {\n        // To make the effects more colorful\n        if(freq > maxF/2) {\n        \tfreq = maxF/2\n        }\n        h = map(freq, 100, maxF/2, 240, 0)\n        s = 1\n        v = map(vol, 0, 100, 0, 1)\n    }\n    \n    for(let i = nLedC - 1; i > 0; i = i - 1) {\n        colors[i] = colors[i-1]\n    }\n    colors[0] = xhsv(h, s, v)\n    setLeds(colors)\n}\n\nlet init = fun(){\n    for(let i = nLedC - 1; i > 0; i = i - 1) {\n        colors[i] = xrgb(0,0,0)\n    }\n    if(nLedC == 16) {\n    \tsetGroupsOf(4)\n    }\n    if(nLedC == 256) {\n    \tsetLinearGroupsOf(16)\n    }\n}\nlet scriptName = \"snake\"";
    
            long _lastUpdate = millis();
            std::unique_ptr<BlinkScript> _blinkScript = nullptr;
    };
}

#endif // NODECONTEXT_H