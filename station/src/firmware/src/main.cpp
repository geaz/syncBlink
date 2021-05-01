#include <Wire.h>
#include <EEPROM.h>
#include <LittleFS.h>

#include "station_context.hpp"

SyncBlink::StationContext stationContext;

void setup()
{
    Serial.begin(74880);
    EEPROM.begin(512);
    LittleFS.begin();

    stationContext.setup();
    
    pinMode(A0, INPUT);         // Mic Input
    pinMode(LED_PIN, OUTPUT);   // LED
}

void loop() 
{
    stationContext.loop();
}