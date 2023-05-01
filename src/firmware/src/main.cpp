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

#ifdef LOG_HEAP
long lastHeapLog;
#endif

void setup()
{
    Serial.begin(74880);
    LittleFS.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(A0, INPUT);

    config.load();
    led.setup(config.Values[F("led_count")]);

    if (config.Values[F("is_hub")])
    {
        Serial.println(F("[MAIN] Starting Hub mode ..."));

        auto wifiModule = std::make_shared<SyncBlink::HubWifiModule>(config, messageBus);
        auto blinkScriptModule = std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, config.Values[F("name")], config.Values[F("type")]);
        auto analyzerModule = std::make_shared<SyncBlink::AnalyzerModule>(messageBus);
        auto displayModule = std::make_shared<SyncBlink::DisplayModule>(messageBus);
        auto scriptModule = std::make_shared<SyncBlink::ScriptModule>(messageBus, config);

        modules.push_back(analyzerModule);
        modules.push_back(displayModule);
        modules.push_back(scriptModule);
        modules.push_back(wifiModule);
        modules.push_back(blinkScriptModule);
        modules.push_back(std::make_shared<SyncBlink::WebModule>(messageBus, *scriptModule.get(), *blinkScriptModule.get(),
                                                                 *analyzerModule.get(), *wifiModule.get(), config));
    }
    else
    {
        if (config.Values[F("is_analyzer")])
        {
            Serial.println(F("[MAIN] Adding Analyzer Module ..."));
            modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(messageBus));
        }

        if (config.Values[F("has_display")])
        {
            Serial.println(F("[MAIN] Adding Display Module ..."));
            modules.push_back(std::make_shared<SyncBlink::DisplayModule>(messageBus));
        }

        if (config.Values[F("is_node")])
        {
            Serial.println(F("[MAIN] Starting Node mode ..."));
            modules.push_back(std::make_shared<SyncBlink::NodeWifiModule>(config, led, messageBus));
            modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, config.Values[F("name")], config.Values[F("type")]));
        }
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

#ifdef LOG_HEAP
    if (millis() - lastHeapLog > 1000)
    {
        Serial.println(ESP.getFreeHeap(), DEC);
        lastHeapLog = millis();
    }
#endif
}