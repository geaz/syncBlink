#ifndef SYNCBLINKWEB_H
#define SYNCBLINKWEB_H

#include "node_manager.hpp"
#include "wifi/station_wifi.hpp"
#include "scripts/script_manager.hpp"

#include <ESP8266WebServer.h>
#include <event_registration.hpp>
#include <network/tcp/tcp_server.hpp>

namespace SyncBlink
{
    typedef std::function<void(float progress, bool isStart, bool isEnd, bool isError, uint64_t targetId)> UploadEvent;

    class SyncBlinkWeb
    {
        public:
            SyncBlinkWeb(StationWifi& stationWifi, ScriptManager& ScriptManager, NodeManager& nodeManager);

            void loop();

            EventRegistration<UploadEvent> uploadListener;

        private:
            void pingNode();
            void renameNode();
            void getMeshInfo();
            void flashMesh();
            void setSource();

            void setWifi();
            void getWifi();
            
            void addScript();
            void saveScript();
            void deleteScript();

            void getScriptList();
            void getScriptContent();

            void getActive();
            void setActive();

            void uploadFirmware();
            void triggerOnUploadEvent(float progress, bool isStart, bool isEnd, bool isError);

            ESP8266WebServer _server;
            StationWifi& _stationWifi;
            ScriptManager& _scriptManager;
            NodeManager& _nodeManager;

            File _firmwareFile;
            uint32_t _firmwareSize = 0;
            uint64_t _firmwareNodeId = 0;
    };
}

#endif // SYNCBLINKWEB_H