#ifndef SYNCBLINKWEB_H
#define SYNCBLINKWEB_H

#include "wifi/station_wifi.hpp"
#include "mod/mod_manager.hpp"
#include "node_manager.hpp"

#include <ESP8266WebServer.h>
#include <network/websocket/socket_server.hpp>

namespace SyncBlink
{
    class SyncBlinkWeb
    {
        public:
            SyncBlinkWeb(StationWifi& stationWifi, ModManager& modManager, NodeManager& nodeManager);

            void loop();

        private:
            void getMeshInfo();
            void flashMesh();

            void setWifi();
            void getWifi();
            
            void addMod();
            void saveMod();
            void deleteMod();

            void getMods();
            void getModContent();

            void getModSettings();
            void setModSettings();

            void uploadFirmware();

            ESP8266WebServer _server;
            StationWifi& _stationWifi;
            ModManager& _modManager;
            NodeManager& _nodeManager;

            File _firmwareFile;
    };
}

#endif // SYNCBLINKWEB_H