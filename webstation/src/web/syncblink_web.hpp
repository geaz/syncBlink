#ifndef SYNCBLINKWEB_H
#define SYNCBLINKWEB_H

#include "wifi/station_wifi.hpp"

#include <ESP8266WebServer.h>
#include <event_registration.hpp>
#include <tcp/tcp_server.hpp>

namespace SyncBlink
{
    typedef std::function<void(float progress, bool isStart, bool isEnd, bool isError, uint64_t targetId)> UploadEvent;
    const std::string FirmwarePath = "/firmware.bin";

    class SyncBlinkWeb
    {
        public:
            SyncBlinkWeb(StationWifi& stationWifi);

            void loop();

            EventRegistration<UploadEvent> uploadListener;

        private:
            void pingNode();
            void renameNode();
            void getMeshInfo();
            void flashMesh();
            void setAnalyzer();
            void setLightMode();

            void setWifi();
            void getWifi();
            
            void addScript();
            void saveScript();
            void deleteScript();

            void getScriptList();
            void getScriptContent();

            void setActiveScript();

            void uploadFirmware();
            void triggerOnUploadEvent(float progress, bool isStart, bool isEnd, bool isError);

            ESP8266WebServer _server;
            StationWifi& _stationWifi;

            File _firmwareFile;
            uint32_t _firmwareSize = 0;
            uint64_t _firmwareNodeId = 0;
    };
}

#endif // SYNCBLINKWEB_H