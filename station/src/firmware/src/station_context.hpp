#ifndef STATIONCONTEXT_H
#define STATIONCONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <network/tcp/tcp_server.hpp>

#include "states/state.hpp"
#include "display/display.hpp"
#include "web/syncblink_web.hpp"
#include "wifi/station_wifi.hpp"
#include "mod/mod_manager.hpp"
#include "node_manager.hpp"

namespace SyncBlink
{
    const uint8_t WifiRomSSIDStart = 0;
    const uint8_t WifiRomSSIDEnd = 32;
    const uint8_t WifiRomSSIDLength = WifiRomSSIDEnd - WifiRomSSIDStart;

    const uint8_t WifiRomPwStart = 32;
    const uint8_t WifiRomPwEnd = 96;
    const uint8_t WifiRomPwLength = WifiRomPwEnd - WifiRomPwStart;

    const uint8_t ModRomStart = 96;
    const uint8_t ModRomEnd = 193;
    const uint8_t ModRomLength = ModRomEnd - ModRomStart;

    class StationContext
    {
        public:
            StationContext();

            void setup();
            void loop();
            void resetState();

            LED& getLed();
            Display& getDisplay();
            ModManager& getModManager();
            SyncBlinkWeb& getWebserver();
            TcpServer& getTcpServer();
            NodeManager& getNodeManager();

            uint64_t getStationId() const;

            std::shared_ptr<State> currentState;

        private:
            void checkException();
            void onMeshDisconnection(uint64_t clientId);
            void onSocketServerCommandReceived(TcpMessage tcpMessage);

            LED _led;
            Display _display;
            StationWifi _wifi;  
            NodeManager _nodeManager;
            ModManager _modManager;
            TcpServer _tcpServer;          
            SyncBlinkWeb _web;

            uint64_t _stationId = SyncBlink::getId();

            #ifdef LOG_HEAP
            long _lastHeapLog;
            #endif
    };
}

#endif // STATIONCONTEXT_H