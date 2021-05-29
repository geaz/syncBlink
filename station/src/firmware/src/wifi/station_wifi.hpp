#ifndef STATIONWIFI_H
#define STATIONWIFI_H

#include <AsyncPing.h>
#include <string>
#include <network/mesh/syncblink_mesh.hpp>

namespace SyncBlink
{
    class StationWifi
    {
        public:
            StationWifi();

            void connectWifi();
            void saveWifi(std::string ssid, std::string pass);

            std::string getSavedSSID();
            std::string getSavedPass();

        private:
            SyncBlinkMesh _mesh;
            AsyncPing _ping;
    };
}

#endif // STATIONWIFI_H