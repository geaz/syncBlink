#ifndef STATIONCONTEXT_H
#define STATIONCONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <tcp/tcp_server.hpp>
#include <mesh/syncblink_mesh.hpp>

#include "states/state.hpp"
#include "display/display.hpp"
#include "scripts/script_manager.hpp"
#include "servers/blink_server.hpp"
#include "servers/api_server.hpp"
#include "node_manager.hpp"

namespace SyncBlink
{
    const uint8_t ScriptRomStart = 96;
    const uint8_t ScriptRomEnd = 193;
    const uint8_t ScriptRomLength = ScriptRomEnd - ScriptRomStart;

    class StationContext
    {
        public:
            StationContext();

            void setup();
            void loop();
            void resetState();

            LED& getLed();
            Display& getDisplay();
            ScriptManager& getScriptManager();
            TcpServer& getBlinkTcpServer();
            NodeManager& getNodeManager();

            uint64_t getStationId() const;

            std::shared_ptr<State> currentState;

        private:
            void checkException();

            LED _led;
            Display _display;
            SyncBlinkMesh _mesh;
            NodeManager _nodeManager;
            ScriptManager _scriptManager;
            BlinkServer _blinkServer;
            ApiServer _apiServer;

            uint64_t _stationId = SyncBlink::getId();

            #ifdef LOG_HEAP
            long _lastHeapLog;
            #endif
    };
}

#endif // STATIONCONTEXT_H