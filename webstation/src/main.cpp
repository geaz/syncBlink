#include <EEPROM.h>
#include <LittleFS.h>

#include "wifi/station_wifi.hpp"
#include "web/syncblink_web.hpp"

#include <mesh/syncblink_mesh.hpp>

SyncBlink::StationWifi _wifi;
SyncBlink::SyncBlinkWeb _web(_wifi);

void setup()
{
    Serial.begin(74880);
    Serial.println("Starting SyncBlink Webstation ...");
    
    EEPROM.begin(512);
    LittleFS.begin();

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0));
    WiFi.softAP("SyncBlink Webstation", SyncBlink::Password);

    _wifi.connectWifi();
}

void loop()
{
    _web.loop();
}