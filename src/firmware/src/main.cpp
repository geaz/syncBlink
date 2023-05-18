#include "core/script_list.hpp"
#include "core/mesh_info.hpp"
#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "modules/analyzer_module.hpp"
#include "modules/blinkscript_module.hpp"
#include "modules/display_module.hpp"
#include "modules/hub_wifi_module.hpp"
#include "modules/node_wifi_module.hpp"
#include "modules/script_broadcast_module.hpp"
#include "modules/web_module.hpp"

#include <LittleFS.h>
#include <Wire.h>
#include <led.hpp>
#include <vector>

SyncBlink::LED led;
SyncBlink::Config config;
SyncBlink::ScriptList scriptList;
SyncBlink::MeshInfo meshInfo(config, scriptList);
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

        modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(messageBus, meshInfo));
        modules.push_back(std::make_shared<SyncBlink::DisplayModule>(messageBus));
        modules.push_back(std::make_shared<SyncBlink::ScriptBroadcastModule>(messageBus, meshInfo, scriptList));
        modules.push_back(std::make_shared<SyncBlink::HubWifiModule>(config, messageBus, scriptList, meshInfo));
        modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, scriptList, config.Values[F("name")], config.Values[F("type")]));
        modules.push_back(std::make_shared<SyncBlink::WebModule>(messageBus, config, scriptList, meshInfo));
    }
    else
    {
        if (config.Values[F("is_analyzer")])
        {
            Serial.println(F("[MAIN] Adding Analyzer Module ..."));
            modules.push_back(std::make_shared<SyncBlink::AnalyzerModule>(messageBus, meshInfo));
        }

        if (config.Values[F("has_display")])
        {
            Serial.println(F("[MAIN] Adding Display Module ..."));
            modules.push_back(std::make_shared<SyncBlink::DisplayModule>(messageBus));
        }

        if (config.Values[F("is_node")])
        {
            Serial.println(F("[MAIN] Starting Node mode ..."));
            modules.push_back(std::make_shared<SyncBlink::NodeWifiModule>(config, led, messageBus, scriptList, meshInfo));
            modules.push_back(std::make_shared<SyncBlink::BlinkScriptModule>(led, messageBus, scriptList, config.Values[F("name")], config.Values[F("type")]));
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
        Serial.printf_P(PSTR("DRAM free: %6d bytes\r\n"), ESP.getFreeHeap());
        lastHeapLog = millis();
    }
#endif
}