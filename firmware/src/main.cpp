#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "modules/analyzer_module.hpp"
#include "modules/display_module.hpp"
#include "modules/script_module.hpp"
#include "modules/blinkscript_module.hpp"
#include "modules/hub_wifi_module.hpp"
#include "modules/node_wifi_module.hpp"

#include <vector>
#include <Wire.h>
#include <led.hpp>
#include <LittleFS.h>

SyncBlink::LED led;
SyncBlink::Config config;
SyncBlink::MessageBus messageBus;
std::vector<std::shared_ptr<SyncBlink::Module>> modules;

void setup()
{
    Serial.begin(74880);
    LittleFS.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(A0, INPUT);
    
    config.load();
    led.setup(config.Values["led_count"]);

    if(config.Values["is_analyzer"].as<bool>())
    {
        Serial.println("[MAIN] Adding Analyzer Module ...");
        modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(messageBus));
    }
    
    if(config.Values["has_display"].as<bool>())
    {
        Serial.println("[MAIN] Adding Display Module ...");
        modules.push_back(std::make_shared<SyncBlink::DisplayModule>(messageBus));
    }

    if (config.Values["is_hub"].as<bool>())
    {
        Serial.println("[MAIN] Starting Hub mode ...");

        auto scriptModule = std::make_shared<SyncBlink::ScriptModule>(messageBus, config);
        modules.push_back(scriptModule);
        
        modules.push_back(std::make_shared<SyncBlink::HubWifiModule>(config, messageBus, *scriptModule.get()));
        modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, scriptModule->getActiveScript()));
    }
    else
    {
        Serial.println("[MAIN] Starting Node mode ...");
        modules.push_back(std::make_shared<SyncBlink::NodeWifiModule>(config, messageBus));
        modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus));
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