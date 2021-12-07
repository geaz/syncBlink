#ifndef NODECONTEXT_H
#define NODECONTEXT_H

#include "node_rom.hpp"
#include "node_connector.hpp"
#include "node_analyzer.hpp"
#include "node_flasher.hpp"

#include <memory>
#include <vector>
#include <led/led.hpp>
#include <blinkscript/blink_script.hpp>
#include <network/mesh/syncblink_mesh.hpp>

namespace SyncBlink
{     
    static const uint8_t SleepSeconds = 30;

    class NodeContext
    {
        public:
            void setup();
            void loop();

        private:
            void checkModeButton();

            LED _led;            
            NodeRom _nodeRom;
            NodeConnector _nodeConnector = NodeConnector(_nodeRom, _led);
            NodeAnalyzer _nodeAnalyzer = NodeAnalyzer(_led, _nodeConnector);
            NodeFlasher _nodeFlasher = NodeFlasher(_nodeConnector.getTcpClient(), _nodeConnector.getTcpServer(), _led);

            #ifdef MODE_PIN
            bool _hasModeButton = true;
            uint8_t _modePin = MODE_PIN;
            #else
            bool _hasModeButton = false;
            uint8_t _modePin = 0;
            #endif

            bool _lastButtonVal = false;
            unsigned long _lastButtonUpdate = 0;
    };
}

#endif // NODECONTEXT_H