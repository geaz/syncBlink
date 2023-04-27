#include "web_module.hpp"

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <sstream>

namespace SyncBlink
{
    WebModule::WebModule(MessageBus& messageBus, ScriptModule& scriptModule, BlinkScriptModule& blinkScriptModule,
                         HubWifiModule& wifiModule, Config& config)
        : _server(80), _messageBus(messageBus), _scriptModule(scriptModule), _blinkScriptModule(blinkScriptModule), _wifiModule(wifiModule),
          _config(config)
    {
        _server.on("/api/mesh/ping", [this]() { pingNode(); });
        _server.on("/api/mesh/rename", [this]() { renameNode(); });
        _server.on("/api/mesh/setNodeWifi", [this]() { setNodeWifi(); });
        _server.on("/api/mesh/info", [this]() { getMeshInfo(); });
        _server.on("/api/mesh/setAnalyzer", [this]() { setAnalyzer(); });
        _server.on("/api/mesh/setLightMode", [this]() { setLightMode(); });

        _server.on("/api/wifi/set", [this]() { setWifi(); });
        _server.on("/api/wifi/get", [this]() { getWifi(); });

        _server.on("/api/scripts/list", [this]() { getScriptList(); });
        _server.on("/api/scripts/get", [this]() { getScriptContent(); });

        _server.on("/api/scripts/add", [this]() { addScript(); });
        _server.on("/api/scripts/save", [this]() { saveScript(); });
        _server.on("/api/scripts/delete", [this]() { deleteScript(); });
        _server.on("/api/scripts/set", [this]() { setActiveScript(); });

        _server.serveStatic("/", LittleFS, "/public/");
        _server.begin();
    }

    void WebModule::loop()
    {
        _server.handleClient();
    }

    void WebModule::setAnalyzer()
    {
        String analyzerIdArg = _server.arg("analyzerId");

        uint64_t analyzerId;
        std::istringstream iss(analyzerIdArg.c_str());
        iss >> analyzerId;

        // BlinkScriptModule Set AnalyzerChange Message
        _server.send(200, "text/plain");
    }

    void WebModule::setLightMode()
    {
        bool lightMode = _server.arg("lightMode") == "true";
        // LightModeChange Message
        _server.send(200, "text/plain");
    }

    void WebModule::pingNode()
    {
        String nodeIdArg = _server.arg("nodeId");

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        // PingMessage
        _server.send(200, "text/plain");
    }

    void WebModule::renameNode()
    {
        String nodeIdArg = _server.arg("nodeId");
        String label = _server.arg("label");

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        //_nodeManager.renameNode(nodeId, std::string(label.c_str()));
        // RenameMessage
        _server.send(200, "text/plain");
    }

    void WebModule::setNodeWifi()
    {
        String nodeIdArg = _server.arg("nodeId");
        bool meshWifi = _server.arg("meshWifi") == "true";

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        // _nodeManager.setWifi(nodeId, meshWifi);
        // NodeWifiChangeMessage
        _server.send(200, "text/plain");
    }

    void WebModule::getMeshInfo()
    {
        String JSON;
        DynamicJsonDocument doc(4096);

        std::string ssid = _config.Values["wifi_ssid"];

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
            nodeJson["isStation"] = node.isStation;
            nodeJson["isAnalyzer"] = node.isAnalyzer;
            nodeJson["isNode"] = node.isNode;
            nodeJson["nodeId"] = iter->first;
            nodeJson["parentId"] = node.parentId;
            nodeJson["ledCount"] = node.ledCount;
            nodeJson["majorVersion"] = node.majorVersion;
            nodeJson["minorVersion"] = node.minorVersion;
            nodeJson["label"] = &node.nodeLabel[0];
            nodeJson["connectedToMeshWifi"] = node.connectedToMeshWifi;

            doc["nodes"][i++] = nodeJson;
            ++iter;
        }

        doc["analyzer"] = _blinkScriptModule.getActiveAnalyzer();
        doc["lightMode"] = _blinkScriptModule.getLightMode();
        doc["ssid"] = ssid.c_str();
        doc["connected"] = WiFi.status() == WL_CONNECTED;

        std::string activeScript = _scriptModule.getActiveScript().Name;
        doc["script"] = activeScript.c_str();

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void WebModule::setWifi()
    {
        _config.Values["wifi_ssid"] = _server.arg("ssid");
        _config.Values["wifi_pw"] = _server.arg("pass");
        _config.save();

        _server.send(200, "application/json");
        delay(500);
        ESP.restart();
    }

    void WebModule::getWifi()
    {
        std::string ssid = _config.Values["wifi_ssid"];
        String JSON;
        
        StaticJsonDocument<256> doc;
        doc["ssid"] = ssid.c_str();
        doc["connected"] = WiFi.status() == WL_CONNECTED;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void WebModule::addScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        _scriptModule.add(scriptName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void WebModule::saveScript()
    {
        std::string body = _server.arg("plain").c_str();
        DynamicJsonDocument script(5000);
        deserializeJson(script, body);

        std::string scriptName = script["name"];
        std::string scriptContent = script["content"];

        _scriptModule.save(scriptName.c_str(), scriptContent.c_str());
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void WebModule::deleteScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        _scriptModule.remove(scriptName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void WebModule::getScriptList()
    {
        String JSON;
        DynamicJsonDocument doc(4096);
        JsonArray files = doc.createNestedArray("scripts");

        std::vector<std::string> scriptList = _scriptModule.getList();
        for (std::string scriptName : scriptList)
            files.add(scriptName.c_str());

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void WebModule::getScriptContent()
    {
        String JSON;
        DynamicJsonDocument doc(5000);

        std::string scriptName = _server.arg("name").c_str();
        Script script = _scriptModule.get(scriptName);

        doc["name"] = script.Name.c_str();
        doc["content"] = script.Content.c_str();
        doc["exists"] = script.Exists;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void WebModule::setActiveScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        if (_scriptModule.get(scriptName).Exists)
        {
            _scriptModule.setActiveScript(scriptName);
            _server.send(200, "application/json", "{ \"saved\": true }");
        }
        else
        {
            _server.send(200, "application/json", "{ \"saved\": false }");
        }
    }
}