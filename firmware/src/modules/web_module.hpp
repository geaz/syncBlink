#ifndef WEBMODULE_H
#define WEBMODULE_H

#include "blinkscript_module.hpp"
#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/script_change.hpp"
#include "core/script.hpp"
#include "hub_wifi_module.hpp"
#include "module.hpp"
#include "script_module.hpp"

#include <ESP8266WebServer.h>
#include <string>
#include <vector>

namespace SyncBlink
{
    class WebModule : public Module
    {
    public:
        WebModule(MessageBus& messageBus, ScriptModule& scriptModule, BlinkScriptModule& blinkScriptModule, HubWifiModule& wifiModule,
                  Config& config);

        void loop();

    private:
        void pingNode();
        void renameNode();
        void getMeshInfo();
        void flashMesh();
        void setAnalyzer();
        void setLightMode();

        void setWifi();
        void getWifi();

        void addScript();
        void saveScript();
        void deleteScript();

        void getScriptList();
        void getScriptContent();

        void setActiveScript();

        ESP8266WebServer _server;
        MessageBus& _messageBus;
        ScriptModule& _scriptModule;
        BlinkScriptModule& _blinkScriptModule;
        HubWifiModule& _wifiModule;
        Config& _config;
    };
}

#endif // WEBMODULE_H