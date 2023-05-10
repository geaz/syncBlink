#ifndef WEBMODULE_H
#define WEBMODULE_H

#include "analyzer_module.hpp"
#include "blinkscript_module.hpp"
#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/script_load.hpp"
#include "core/script.hpp"
#include "core/script_list.hpp"
#include "core/mesh_info.hpp"
#include "hub_wifi_module.hpp"
#include "module.hpp"

#include <ESP8266WebServer.h>
#include <sstream>
#include <string>
#include <vector>

namespace SyncBlink
{
    class WebModule : public Module
    {
    public:
        WebModule(MessageBus& messageBus, Config& config, ScriptList& scriptList, MeshInfo& meshInfo);

        void loop() override;

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
        void saveScript(bool isBytecode);
        void deleteScript();

        void getScriptList();
        void getScriptContent();

        void setActiveScript();

        template <typename T> std::string toString(const T& value)
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        ESP8266WebServer _server;
        MessageBus& _messageBus;
        Config& _config;
        ScriptList& _scriptList;
        MeshInfo& _meshInfo;

        Script _script;
        uint64_t _activeAnalzyerId;
    };
}

#endif // WEBMODULE_H