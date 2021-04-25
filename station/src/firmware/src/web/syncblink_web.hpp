#ifndef SYNCBLINKWEB_H
#define SYNCBLINKWEB_H

#include <ESP8266WebServer.h>
#include "wifi/station_wifi.hpp"
#include "mod/mod_manager.hpp"
#include "node_manager.hpp"

namespace SyncBlink
{
    class SyncBlinkWeb
    {
        public:
            SyncBlinkWeb(StationWifi& stationWifi, ModManager& modManager, NodeManager& nodeManager);

            void loop();

        private:
            void getMeshInfo();

            void setWifi();
            void getWifi();
            
            void addMod();
            void saveMod();
            void deleteMod();

            void getMods();
            void getModContent();

            void getModSettings();
            void setModSettings();

            ESP8266WebServer _server;
            StationWifi& _stationWifi;
            ModManager& _modManager;
            NodeManager& _nodeManager;

    };
}

#endif // SYNCBLINKWEB_H