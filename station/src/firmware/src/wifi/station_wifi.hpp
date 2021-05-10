#ifndef STATIONWIFI_H
#define STATIONWIFI_H

#include <string>
#include <network/mesh/syncblink_mesh.hpp>

namespace SyncBlink
{
    class StationWifi
    {
        public:
            StationWifi();

            void keepAlive();
            void connectWifi();
            void saveWifi(std::string ssid, std::string pass);

            std::string getSavedSSID();
            std::string getSavedPass();

        private:
            SyncBlinkMesh _mesh;
            unsigned long _lastPing = millis();
    };
}

#endif // STATIONWIFI_H