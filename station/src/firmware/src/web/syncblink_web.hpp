#ifndef SYNCBLINKWEB_H
#define SYNCBLINKWEB_H

#include "wifi/station_wifi.hpp"
#include "mod/mod_manager.hpp"
#include "node_manager.hpp"

#include <ESP8266WebServer.h>
#include <event_registration.hpp>
#include <network/websocket/socket_server.hpp>

namespace SyncBlink
{
    typedef std::function<void(float progress, bool isStart, bool isEnd, bool isError)> UploadEvent;

    class SyncBlinkWeb
    {
        public:
            SyncBlinkWeb(StationWifi& stationWifi, ModManager& modManager, NodeManager& nodeManager);

            void loop();

            EventRegistration<UploadEvent> uploadListener;

        private:
            void renameNode();
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
            void triggerOnUploadEvent(float progress, bool isStart, bool isEnd, bool isError);

            ESP8266WebServer _server;
            StationWifi& _stationWifi;
            ModManager& _modManager;
            NodeManager& _nodeManager;

            File _firmwareFile;
            uint32_t _firmwareSize = 0;
    };
}

#endif // SYNCBLINKWEB_H