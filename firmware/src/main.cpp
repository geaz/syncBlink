#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "modules/analyzer_module.hpp"
#include "modules/blinkscript_module.hpp"
#include "modules/display_module.hpp"
#include "modules/hub_wifi_module.hpp"
#include "modules/node_wifi_module.hpp"
#include "modules/script_module.hpp"
#include "modules/web_module.hpp"

#include <LittleFS.h>
#include <Wire.h>
#include <led.hpp>
#include <vector>

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

    if (config.Values["is_analyzer"])
    {
        Serial.println("[MAIN] Adding Analyzer Module ...");
        modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(messageBus));
    }

    if (config.Values["has_display"])
    {
        Serial.println("[MAIN] Adding Display Module ...");
        modules.push_back(std::make_shared<SyncBlink::DisplayModule>(messageBus));
    }

    if (config.Values["is_hub"])
    {
        Serial.println("[MAIN] Starting Hub mode ...");

        auto scriptModule = std::make_shared<SyncBlink::ScriptModule>(messageBus, config);
        auto wifiModule = std::make_shared<SyncBlink::HubWifiModule>(config, messageBus, *scriptModule.get());
        auto blinkScriptModule = std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, scriptModule->getActiveScript());

        modules.push_back(scriptModule);
        modules.push_back(wifiModule);
        modules.push_back(blinkScriptModule);
        modules.push_back(
            std::make_shared<SyncBlink::WebModule>(messageBus, *scriptModule.get(), *blinkScriptModule.get(), *wifiModule.get(), config));
    }
    else
    {
        Serial.println("[MAIN] Starting Node mode ...");
        modules.push_back(std::make_shared<SyncBlink::NodeWifiModule>(config, messageBus));
        modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus));
    }

    for (auto module : modules)
    {
        module->setup();
    }
}

void loop()
{
    for (auto module : modules)
    {
        module->loop();
    }
    led.loop();
}