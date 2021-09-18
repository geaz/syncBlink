#ifndef STATIONCONTEXT_H
#define STATIONCONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <tcp/tcp_server.hpp>
#include <mesh/syncblink_mesh.hpp>

#include "states/state.hpp"
#include "display/display.hpp"
#include "scripts/script_manager.hpp"
#include "node_manager.hpp"
#include "serial_api.hpp"

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
            TcpServer& getTcpServer();
            NodeManager& getNodeManager();
            ScriptManager& getScriptManager();
            uint64_t getStationId() const;

            std::shared_ptr<State> currentState;

        private:
            void checkException();

            LED _led;
            TcpServer _tcpServer = TcpServer(81);
            Display _display;
            SyncBlinkMesh _mesh;
            NodeManager _nodeManager;
            ScriptManager _scriptManager;

            uint64_t _stationId = SyncBlink::getId();

            #ifdef LOG_HEAP
            long _lastHeapLog;
            #endif
    };
}

#endif // STATIONCONTEXT_H