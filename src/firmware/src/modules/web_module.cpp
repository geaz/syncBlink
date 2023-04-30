#include "web_module.hpp"
#include "core/message/messages/node_command.hpp"
#include "core/message/messages/analyzer_change.hpp"

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <sstream>

namespace SyncBlink
{
    WebModule::WebModule(MessageBus& messageBus, ScriptModule& scriptModule, BlinkScriptModule& blinkScriptModule,
                         AnalyzerModule& analyzerModule, HubWifiModule& wifiModule, Config& config)
        : _server(80), _messageBus(messageBus), _scriptModule(scriptModule), _blinkScriptModule(blinkScriptModule),
          _analyzerModule(analyzerModule), _wifiModule(wifiModule), _config(config)
    {
        _server.on(F("/api/mesh/ping"), [this]() { pingNode(); });
        _server.on(F("/api/mesh/rename"), [this]() { renameNode(); });
        _server.on(F("/api/mesh/setNodeWifi"), [this]() { setNodeWifi(); });
        _server.on(F("/api/mesh/info"), [this]() { getMeshInfo(); });
        _server.on(F("/api/mesh/setAnalyzer"), [this]() { setAnalyzer(); });
        _server.on(F("/api/mesh/setLightMode"), [this]() { setLightMode(); });

        _server.on(F("/api/wifi/set"), [this]() { setWifi(); });
        _server.on(F("/api/wifi/get"), [this]() { getWifi(); });

        _server.on(F("/api/scripts/list"), [this]() { getScriptList(); });
        _server.on(F("/api/scripts/get"), [this]() { getScriptContent(); });

        _server.on(F("/api/scripts/add"), [this]() { addScript(); });
        _server.on(F("/api/scripts/save"), [this]() { saveScript(); });
        _server.on(F("/api/scripts/delete"), [this]() { deleteScript(); });
        _server.on(F("/api/scripts/set"), [this]() { setActiveScript(); });

        _server.serveStatic("/", LittleFS, "/public/");
        _server.begin();
    }

    void WebModule::loop()
    {
        _server.handleClient();
    }

    void WebModule::setAnalyzer()
    {
        String analyzerIdArg = _server.arg(F("analyzerId"));

        uint64_t analyzerId;
        std::istringstream iss(analyzerIdArg.c_str());
        iss >> analyzerId;

        Messages::AnalyzerChange msg;
        msg.analyzerId = analyzerId;

        _messageBus.trigger(msg);
        _server.send(200, "text/plain");
    }

    void WebModule::setLightMode()
    {
        bool lightMode = _server.arg(F("lightMode")) == F("true");
        // LightModeChange Message
        _server.send(200, "text/plain");
    }

    void WebModule::pingNode()
    {
        String nodeIdArg = _server.arg(F("nodeId"));

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        Messages::NodeCommand msg;
        msg.recipientId = nodeId;
        msg.commandType = Messages::NodeCommandType::Ping;

        _messageBus.trigger(msg);
        _server.send(200, "text/plain");
    }

    void WebModule::renameNode()
    {
        String nodeIdArg = _server.arg(F("nodeId"));
        String label = _server.arg(F("label"));

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        Messages::NodeCommand msg;
        msg.recipientId = nodeId;
        msg.commandType = Messages::NodeCommandType::Rename;
        msg.commandInfo.stringInfo1 = label.c_str();

        _messageBus.trigger(msg);
        _server.send(200, "text/plain");
    }

    void WebModule::setNodeWifi()
    {
        String nodeIdArg = _server.arg(F("nodeId"));
        bool meshWifi = _server.arg(F("meshWifi")) == F("true");

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        std::string ssid = _config.Values[F("wifi_ssid")];
        std::string pw = _config.Values[F("wifi_pw")];

        Messages::NodeCommand msg;
        msg.recipientId = nodeId;
        msg.commandType = Messages::NodeCommandType::WifiChange;
        msg.commandInfo.flag = meshWifi;
        msg.commandInfo.stringInfo1 = ssid;
        msg.commandInfo.stringInfo2 = pw;

        _messageBus.trigger(msg);
        _server.send(200, "text/plain");
    }

    void WebModule::getMeshInfo()
    {
        String JSON;
        DynamicJsonDocument doc(4096);

        std::string ssid = _config.Values[F("wifi_ssid")];

        auto stationInfo = _wifiModule.getStationInfo();
        auto connectedNodes = _wifiModule.getConnectedNodes();
        connectedNodes[std::get<0>(stationInfo)] = std::get<1>(stationInfo);

        auto i = 0;
        auto iter = connectedNodes.begin();
        auto endIter = connectedNodes.end();

        for (; iter != endIter;)
        {
            auto node = iter->second;
            StaticJsonDocument<512> nodeJson;
            nodeJson[F("isStation")] = node.isStation;
            nodeJson[F("isAnalyzer")] = node.isAnalyzer;
            nodeJson[F("isNode")] = node.isNode;
            nodeJson[F("nodeId")] = iter->first;
            nodeJson[F("parentId")] = node.parentId;
            nodeJson[F("ledCount")] = node.ledCount;
            nodeJson[F("majorVersion")] = node.majorVersion;
            nodeJson[F("minorVersion")] = node.minorVersion;
            nodeJson[F("label")] = &node.nodeLabel[0];
            nodeJson[F("connectedToMeshWifi")] = node.connectedToMeshWifi;

            doc[F("nodes")][i++] = nodeJson;
            ++iter;
        }

        doc[F("analyzer")] = _analyzerModule.getActiveAnalyzer();
        doc[F("lightMode")] = _blinkScriptModule.getLightMode();
        doc[F("ssid")] = ssid.c_str();
        doc[F("connected")] = WiFi.status() == WL_CONNECTED;

        std::string activeScript = _scriptModule.getActiveScript().Name;
        doc[F("script")] = activeScript.c_str();

        serializeJson(doc, JSON);
        _server.send(200, F("application/json"), JSON);
    }

    void WebModule::setWifi()
    {
        _config.Values[F("wifi_ssid")] = _server.arg(F("ssid"));
        _config.Values[F("wifi_pw")] = _server.arg(F("pass"));
        _config.save();

        _server.send(200, "application/json");
        delay(500);
        ESP.restart();
    }

    void WebModule::getWifi()
    {
        std::string ssid = _config.Values[F("wifi_ssid")];
        String JSON;
        
        StaticJsonDocument<256> doc;
        doc[F("ssid")] = ssid.c_str();
        doc[F("connected")] = WiFi.status() == WL_CONNECTED;

        serializeJson(doc, JSON);
        _server.send(200, F("application/json"), JSON);
    }

    void WebModule::addScript()
    {
        std::string scriptName = _server.arg(F("name")).c_str();

        _scriptModule.add(scriptName);
        _server.send(200, F("application/json"), "{ \"saved\": true }");
    }

    void WebModule::saveScript()
    {
        std::string body = _server.arg(F("plain")).c_str();
        DynamicJsonDocument script(5000);
        deserializeJson(script, body);

        std::string scriptName = script[F("name")];
        std::string scriptContent = script[F("content")];

        _scriptModule.save(scriptName.c_str(), scriptContent.c_str());
        _server.send(200, F("application/json"), F("{ \"saved\": true }"));
    }

    void WebModule::deleteScript()
    {
        std::string scriptName = _server.arg(F("name")).c_str();

        _scriptModule.remove(scriptName);
        _server.send(200, F("application/json"), F("{ \"saved\": true }"));
    }

    void WebModule::getScriptList()
    {
        String JSON;
        DynamicJsonDocument doc(4096);
        JsonArray files = doc.createNestedArray(F("scripts"));

        std::vector<std::string> scriptList = _scriptModule.getList();
        for (std::string scriptName : scriptList)
            files.add(scriptName.c_str());

        serializeJson(doc, JSON);
        _server.send(200, F("application/json"), JSON);
    }

    void WebModule::getScriptContent()
    {
        String JSON;
        DynamicJsonDocument doc(5000);

        std::string scriptName = _server.arg(F("name")).c_str();
        Script script = _scriptModule.get(scriptName);

        doc[F("name")] = script.Name.c_str();
        doc[F("content")] = script.Content.c_str();
        doc[F("exists")] = script.Exists;

        serializeJson(doc, JSON);
        _server.send(200, F("application/json"), JSON);
    }

    void WebModule::setActiveScript()
    {
        std::string scriptName = _server.arg(F("name")).c_str();

        if (_scriptModule.get(scriptName).Exists)
        {
            _scriptModule.setActiveScript(scriptName);
            _server.send(200, F("application/json"), F("{ \"saved\": true }"));
        }
        else
        {
            _server.send(200, F("application/json"), F("{ \"saved\": false }"));
        }
    }
}