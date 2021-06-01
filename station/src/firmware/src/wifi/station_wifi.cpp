#include "station_wifi.hpp"
#include "station_context.hpp"

#include <EEPROM.h>

namespace SyncBlink
{
    StationWifi::StationWifi()
    {
        _mesh.startMesh();
        #ifdef DEBUG_STATIONWIFI
        _ping.on(true,[](const AsyncPingResponse& response){
            IPAddress addr(response.addr);
            Serial.printf("[WIFI] Ping result from %s - sent %d recevied %d\n", addr.toString().c_str(), response.total_sent, response.total_recv);
            return true;
        });
        #endif
    }

    void StationWifi::connectWifi()
    {
        #ifdef DEBUG_STATIONWIFI
        Serial.println("[WIFI] Connecting to Network ...");
        #endif
        
        std::string ssid = getSavedSSID();        
        std::string password = getSavedPass();

        if (ssid.length() > 1 ) 
        {
            WiFi.begin(ssid.c_str(), password.c_str());
            
            #ifdef DEBUG_STATIONWIFI
            Serial.println("[WIFI] Waiting for Wifi to connect (30 sec timeout)...");
            #endif
            if(WiFi.waitForConnectResult(30000) == WL_CONNECTED)
            {
                #ifdef DEBUG_STATIONWIFI
                Serial.println("[WIFI] Connected!");
                #endif
            }
            else
            {
                #ifdef DEBUG_STATIONWIFI
                Serial.println("[WIFI] Couldn't connect to network!");
                #endif
                WiFi.disconnect();
            }
        }
    }

    void StationWifi::saveWifi(std::string ssid, std::string pass)
    {
        if (ssid.length() > 0 && pass.length() > 0) 
        {
            #ifdef DEBUG_STATIONWIFI
            Serial.println("[WIFI] Clearing SSID ...");
            #endif
            for (int i = SyncBlink::WifiRomSSIDStart; i < SyncBlink::WifiRomPwEnd; ++i) { EEPROM.write(i, 0); }
        }

        #ifdef DEBUG_STATIONWIFI
        Serial.printf("[WIFI] Writing SSID (%s) ...\n", ssid.c_str());
        #endif
        for (uint i = 0; i < ssid.length(); ++i) EEPROM.write(SyncBlink::WifiRomSSIDStart + i, ssid[i]);

        #ifdef DEBUG_STATIONWIFI
        Serial.printf("[WIFI] Writing Password (%s) ...\n", pass.c_str());
        #endif
        for (uint i = 0; i < pass.length(); ++i) EEPROM.write(SyncBlink::WifiRomPwStart + i, pass[i]);

        EEPROM.commit();
        ESP.restart();
    }

    std::string StationWifi::getSavedSSID()
    {
        int i = 0;
        char data[SyncBlink::WifiRomSSIDLength]{};
        char curChar = char(EEPROM.read(i + SyncBlink::WifiRomSSIDStart));

        while (i < SyncBlink::WifiRomSSIDLength && curChar != '\0' && curChar >= 32 && curChar <= 126)
        {
            curChar = char(EEPROM.read(i + SyncBlink::WifiRomSSIDStart));
            data[i] = curChar;
            i++;
        }
        std::string ssid(data);
        #ifdef DEBUG_STATIONWIFI
        Serial.printf("[WIFI] SSID: %s\n", ssid.c_str());
        #endif

        return ssid;
    }

    std::string StationWifi::getSavedPass()
    {
        int i = 0;
        char data[SyncBlink::WifiRomPwLength]{};
        char curChar = char(EEPROM.read(i + SyncBlink::WifiRomPwStart));

        while (i < SyncBlink::WifiRomPwLength && curChar != '\0' && curChar >= 32 && curChar <= 126)
        {
            curChar = char(EEPROM.read(i + SyncBlink::WifiRomPwStart));
            data[i] = curChar;
            i++;
        }
        std::string password(data);
        #ifdef DEBUG_STATIONWIFI
        Serial.printf("[WIFI] Password: %s\n", password.c_str());
        #endif

        return password;
    }
}