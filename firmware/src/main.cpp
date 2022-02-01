#include "core/config/config.hpp"
#include "core/event/event_bus.hpp"
#include "core/scripting/script_manager.hpp"
#include "modules/analyzer_module.hpp"
#include "modules/display_module.hpp"
#include "modules/script_module.hpp"

#include <vector>
#include <EEPROM.h>
#include <LittleFS.h>
#include <Wire.h>
#include <led.hpp>

SyncBlink::LED led;
SyncBlink::Config config;
SyncBlink::EventBus eventBus;
SyncBlink::ScriptManager scriptManager(eventBus, config);
std::vector<std::shared_ptr<SyncBlink::Module>> modules;

void setup()
{
    Serial.begin(74880);
    EEPROM.begin(512);
    LittleFS.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(A0, INPUT);

    config.load();
    led.setup(config.Values["led_count"]);

    if(config.Values["is_analyzer"] == "true")
    {
        Serial.println("[MAIN] Adding Analyzer Module ...");
        modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(eventBus));
    }
    if(config.Values["has_display"] == "true")
    {
        Serial.println("[MAIN] Adding Display Module ...");
        modules.push_back(std::make_shared<SyncBlink::DisplayModule>(eventBus));
    }

    if (config.Values["is_hub"] == "true")
    {
        Serial.println("[MAIN] Starting Hub mode ...");
        modules.push_back(std::make_shared<SyncBlink::ScriptModule>(led, eventBus, scriptManager.getActiveScript()));
    }
    else
    {
        Serial.println("[MAIN] Starting Node mode ...");
    }

    for (auto module: modules) {
        module->setup();
    }
}

void loop()
{
    for (auto module: modules) {
        module->loop();
    }
    led.loop();
}