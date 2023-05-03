#ifndef WEBMODULE_H
#define WEBMODULE_H

#include "analyzer_module.hpp"
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
#include <sstream>

namespace SyncBlink
{
    class WebModule : public Module
    {
    public:
        WebModule(MessageBus& messageBus, ScriptModule& scriptModule, BlinkScriptModule& blinkScriptModule, AnalyzerModule& analyzerModule,
                  HubWifiModule& wifiModule, Config& config);

        void loop();

    private:
        void pingNode();
        void renameNode();
        void setNodeWifi();
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

        template<typename T>
        std::string toString(const T& value)
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        ESP8266WebServer _server;
        MessageBus& _messageBus;
        ScriptModule& _scriptModule;
        BlinkScriptModule& _blinkScriptModule;
        AnalyzerModule& _analyzerModule;
        HubWifiModule& _wifiModule;
        Config& _config;

        Script _script;
        uint64_t _activeAnalzyerId;
    };
}

#endif // WEBMODULE_H