#include <Wire.h>
#include <EEPROM.h>
#include <LittleFS.h>

#include "config/config.hpp"
#include "operation_modes/operation_mode.hpp"
#include "operation_modes/hub_mode.hpp"
#include "operation_modes/node_mode.hpp"

SyncBlink::Config config;
std::unique_ptr<SyncBlink::OperationMode> operationMode;

void setup()
{
    Serial.begin(74880);
    EEPROM.begin(512);
    LittleFS.begin();

    config.load();

    pinMode(LED_PIN, OUTPUT);
    if(config.Values["mic_pin"] != nullptr) pinMode(config.Values["mic_pin"], INPUT);

    if(config.Values["is_hub"] == "true")
    {
        Serial.println("[MAIN] Starting Hub mode ...");
        operationMode = std::unique_ptr<SyncBlink::OperationMode>(new SyncBlink::HubMode(config));
        operationMode->setup();
    }
    else
    {
        Serial.println("[MAIN] Starting Node mode ...");
        operationMode = std::unique_ptr<SyncBlink::OperationMode>(new SyncBlink::NodeMode(config));
        operationMode->setup();
    }
}

void loop() 
{
    operationMode->loop();
}