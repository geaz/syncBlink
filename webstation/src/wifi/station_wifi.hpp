#ifndef STATIONWIFI_H
#define STATIONWIFI_H

#include <string>
#include <ESP8266WiFi.h>

namespace SyncBlink
{    
    const uint8_t WifiRomSSIDStart = 0;
    const uint8_t WifiRomSSIDEnd = 32;
    const uint8_t WifiRomSSIDLength = WifiRomSSIDEnd - WifiRomSSIDStart;

    const uint8_t WifiRomPwStart = 32;
    const uint8_t WifiRomPwEnd = 96;
    const uint8_t WifiRomPwLength = WifiRomPwEnd - WifiRomPwStart;

    class StationWifi
    {
        public:
            void connectWifi();
            void saveWifi(std::string ssid, std::string pass);

            std::string getSavedSSID();
            std::string getSavedPass();
    };
}

#endif // STATIONWIFI_H