#include <EEPROM.h>
#include <LittleFS.h>

#include "wifi/station_wifi.hpp"
#include "web/syncblink_web.hpp"

SyncBlink::StationWifi _wifi;
SyncBlink::SyncBlinkWeb _web(_wifi);

void setup()
{
    Serial.begin(74880);
    Serial.println("Starting SyncBlink Webstation ...");
    
    EEPROM.begin(512);
    LittleFS.begin();

    _wifi.connectWifi();
}

void loop()
{
    _web.loop();
}