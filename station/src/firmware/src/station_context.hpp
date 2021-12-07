#ifndef STATIONCONTEXT_H
#define STATIONCONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <network/tcp/tcp_server.hpp>
#include <network/tcp/udp_discover.hpp>

#include "states/state.hpp"
#include "display/display.hpp"
#include "web/syncblink_web.hpp"
#include "wifi/station_wifi.hpp"
#include "scripts/script_manager.hpp"
#include "node_manager.hpp"

namespace SyncBlink
{
    const uint8_t WifiRomSSIDStart = 0;
    const uint8_t WifiRomSSIDEnd = 32;

    const uint8_t WifiRomPwStart = 32;
    const uint8_t WifiRomPwEnd = 96;

    const uint8_t ScriptRomStart = 96;
    const uint8_t ScriptRomEnd = 192;

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
            SyncBlinkWeb& getWebserver();
            TcpServer& getTcpServer();
            NodeManager& getNodeManager();

            uint64_t getStationId() const;

            std::shared_ptr<State> currentState;

        private:
            void checkException();

            LED _led;
            TcpServer _tcpServer;
            UdpDiscover _udpDiscover;
            Display _display;
            StationWifi _wifi;  
            NodeManager _nodeManager;
            ScriptManager _ScriptManager;       
            SyncBlinkWeb _web;

            uint64_t _stationId = SyncBlink::getId();

            #ifdef LOG_HEAP
            long _lastHeapLog;
            #endif
    };
}

#endif // STATIONCONTEXT_H