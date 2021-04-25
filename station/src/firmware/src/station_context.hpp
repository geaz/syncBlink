#ifndef STATIONCONTEXT_H
#define STATIONCONTEXT_H

#include <memory>
#include <led/led.hpp>
#include <network/websocket/socket_server.hpp>

#include "states/state.hpp"
#include "display/display.hpp"
#include "web/syncblink_web.hpp"
#include "web/node_manager.hpp"
#include "wifi/station_wifi.hpp"
#include "mod/mod_manager.hpp"

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
    
    const uint8_t SourceRom = 193;

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
            StationWifi& getStationWifi();
            SocketServer& getSocketServer();
            NodeManager& getNodeManager();

            std::shared_ptr<State> currentState;

        private:
            void checkReset();
            void startMeshCount();
            void onSocketServerMeshConnection();
            void onSocketServerCommandReceived(Client::Message message);

            LED _led;
            Display _display;
            StationWifi _wifi;
            ModManager _modManager;
            NodeManager _nodeManager;
            SocketServer _socketServer;
            
            SyncBlinkWeb _web;
    };
}

#endif // STATIONCONTEXT_H