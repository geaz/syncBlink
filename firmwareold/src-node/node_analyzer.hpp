#ifndef NODEANALYZER_H
#define NODEANALYZER_H

#include "node_connector.hpp"

#include <memory>
#include <led/led.hpp>
#include <audio/frequency_analyzer.hpp>
#include <network/mesh/syncblink_mesh.hpp>
#include <blinkscript/blink_script.hpp>

namespace SyncBlink
{
    class NodeAnalyzer
    {
    public:
        NodeAnalyzer(LED& led, NodeConnector& nodeConnector);

        void loop();

        void toggleLightMode();
        bool runsStandalone();

    private:    
        void checkNewScript();
        void checkRunAnalyzer();

        void onSocketClientScriptReceived(std::string script);
        void onAnalyzerResultReceived(AudioAnalyzerMessage message);
        void onSocketServerMessageReceived(Message message);

        LED& _led;
        NodeConnector& _nodeConnector;
        FrequencyAnalyzer _frequencyAnalyzer;

        bool _newScript = false;
        bool _lightMode = false;
        bool _runStandalone = false;
        uint64_t _activeAnalyzer = 0;
        uint64_t _lastLedUpdate = millis();

        std::string _currentScript;
        std::string _standaloneScript = "let h=0\nlet s=0\nlet v=0\nlet colors = []\n\nlet update = fun(delta) {\n\tif(vol == 0 || freq == 0 || (vol < lVol * 1.05 && v > 0.25)){\n        if(v > 0.025){ v = v - 0.025 }\n        else{ v = 0 }\n    } else {\n        // To make the effects more colorful\n        if(freq > maxF/2) {\n        \tfreq = maxF/2\n        }\n        h = map(freq, 100, maxF/2, 240, 0)\n        s = 1\n        v = map(vol, 0, 100, 0, 1)\n    }\n    \n    for(let i = nLedC - 1; i > 0; i = i - 1) {\n        colors[i] = colors[i-1]\n    }\n    colors[0] = xhsv(h, s, v)\n    setLeds(colors)\n}\n\nlet init = fun(){\n    for(let i = nLedC - 1; i > 0; i = i - 1) {\n        colors[i] = xrgb(0,0,0)\n    }\n    if(nLedC == 16) {\n    \tsetGroupsOf(4)\n    }\n    if(nLedC == 256) {\n    \tsetLinearGroupsOf(16)\n    }\n}\nlet scriptName = \"snake\"";

        long _lastUpdate = millis();
        std::unique_ptr<BlinkScript> _blinkScript = nullptr;
    };
}

#endif // NODEANALYZER_H