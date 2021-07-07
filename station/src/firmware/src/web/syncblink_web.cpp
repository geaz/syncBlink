#include "syncblink_web.hpp"

#include <sstream>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <network/tcp/messages/audio_analyzer_message.hpp>

namespace SyncBlink
{
    SyncBlinkWeb::SyncBlinkWeb(StationWifi& stationWifi, ScriptManager& ScriptManager, NodeManager& nodeManager)
        : _server(80), _stationWifi(stationWifi), _scriptManager(ScriptManager), _nodeManager(nodeManager)
    {
        _server.on("/api/mesh/ping", [this]() { pingNode(); });
        _server.on("/api/mesh/rename", [this]() { renameNode(); });
        _server.on("/api/mesh/info", [this]() { getMeshInfo(); });
        _server.on("/api/mesh/setAnalyzer", [this]() { setAnalyzer(); });
        _server.on("/api/mesh/flash", HTTP_POST, 
            [this]() { 
                _server.sendHeader("Connection", "close");
                _server.send(200, "text/plain");
            }, 
            [this]() { uploadFirmware(); yield(); });

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

    void SyncBlinkWeb::loop()
    {
        _server.handleClient();
    }

    void SyncBlinkWeb::setAnalyzer()
    {
        String analyzerIdArg = _server.arg("analyzerId");

        uint64_t analyzerId;
        std::istringstream iss(analyzerIdArg.c_str());
        iss >> analyzerId;

        _nodeManager.setAnalyzer(analyzerId);
        _server.send(200, "text/plain");
    }

    void SyncBlinkWeb::pingNode()
    {
        String nodeIdArg = _server.arg("nodeId");

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        _nodeManager.pingNode(nodeId);
        _server.send(200, "text/plain");
    }

    void SyncBlinkWeb::renameNode()
    {
        String nodeIdArg = _server.arg("nodeId");
        String label = _server.arg("label");

        uint64_t nodeId;
        std::istringstream iss(nodeIdArg.c_str());
        iss >> nodeId;

        _nodeManager.renameNode(nodeId, std::string(label.c_str()));
        _server.send(200, "text/plain");
    }

    void SyncBlinkWeb::getMeshInfo()
    {
        String JSON;
        DynamicJsonDocument doc(1024);
        
        auto connectedNodes = _nodeManager.getConnectedNodes();
        for(uint32_t i = 0; i < connectedNodes.size(); i++)
        {
            StaticJsonDocument<500> nodeJson;
            nodeJson["isStation"] = connectedNodes[i].isStation;
            nodeJson["isAnalyzer"] = connectedNodes[i].isAnalyzer;
            nodeJson["isNode"] = connectedNodes[i].isNode;
            nodeJson["nodeId"] = connectedNodes[i].nodeId;
            nodeJson["parentId"] = connectedNodes[i].parentId;
            nodeJson["ledCount"] = connectedNodes[i].ledCount;
            nodeJson["majorVersion"] = connectedNodes[i].majorVersion;
            nodeJson["minorVersion"] = connectedNodes[i].minorVersion;
            nodeJson["label"] = &connectedNodes[i].nodeLabel[0];

            doc["nodes"][i] = nodeJson;
        }
        doc["analyzer"] = _nodeManager.getActiveAnalyzer();

        std::string activeScript = _scriptManager.getActiveScript();
        doc["script"] = activeScript.c_str();

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::setWifi()
    {
        String ssid = _server.arg("ssid");
        String pass = _server.arg("pass");
        _stationWifi.saveWifi(ssid.c_str(), pass.c_str());
        _server.send(200, "application/json");
    }

    void SyncBlinkWeb::getWifi()
    {
        std::string ssid = _stationWifi.getSavedSSID();

        String JSON;
        StaticJsonDocument<500> doc;
        doc["ssid"] = ssid.c_str();
        doc["connected"] = WiFi.status() == WL_CONNECTED;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }    

    void SyncBlinkWeb::addScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        _scriptManager.add(scriptName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::saveScript()
    {
        String body = _server.arg("plain");
        StaticJsonDocument<5000> script;
        deserializeJson(script, body);

        String scriptName = script["name"];
        String scriptContent = script["content"];

        _scriptManager.save(scriptName.c_str(), scriptContent.c_str());
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::deleteScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        _scriptManager.remove(scriptName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::getScriptList()
    {
        String JSON;
        StaticJsonDocument<1000> jsonBuffer;
        JsonArray files = jsonBuffer.createNestedArray("scripts");

        std::vector<std::string> scriptList = _scriptManager.getList();
        for(std::string scriptName : scriptList)
            files.add(scriptName.c_str());

        serializeJson(jsonBuffer, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::getScriptContent()
    {
        String JSON, scriptContent;
        StaticJsonDocument<5000> doc;    

        std::string scriptName = _server.arg("name").c_str();
        Script script = _scriptManager.get(scriptName);
        
        doc["name"] = script.Name.c_str();
        doc["content"] = script.Content.c_str();
        doc["exists"] = script.Exists;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::setActiveScript()
    {
        std::string scriptName = _server.arg("name").c_str();

        if(_scriptManager.get(scriptName).Exists) {
            _scriptManager.setActiveScript(scriptName);
            _server.send(200, "application/json", "{ \"saved\": true }");
        }
        else {
            _server.send(200, "application/json", "{ \"saved\": false }");
        }        
    }

    void SyncBlinkWeb::uploadFirmware()
    {
        HTTPUpload& upload = _server.upload();
        if(upload.status == UPLOAD_FILE_START)
        {                        
            LittleFS.remove(FirmwarePath.c_str()); // Remove old node firmware

            String nodeIdArg = _server.arg("nodeId");
            std::istringstream iss(nodeIdArg.c_str());
            iss >> _firmwareNodeId;
            
            _firmwareSize = std::atoi(_server.arg("size").c_str());
            _firmwareFile = LittleFS.open(FirmwarePath.c_str(), "a");
            triggerOnUploadEvent(0, true, false, false);
        }
        if (upload.status == UPLOAD_FILE_WRITE)
        {
            _firmwareFile.write(upload.buf, upload.currentSize);
            triggerOnUploadEvent((float)upload.totalSize/_firmwareSize, false, false, false);
        } 
        else if (upload.status == UPLOAD_FILE_END)
        {            
            _firmwareFile.close();
            triggerOnUploadEvent(1, false, true, false);
        }
        else if(upload.status == UPLOAD_FILE_ABORTED)
        {
            _firmwareFile.close();
            LittleFS.remove(FirmwarePath.c_str());
            triggerOnUploadEvent(1, false, false, true);
        }
    }

    void SyncBlinkWeb::triggerOnUploadEvent(float progress, bool isStart, bool isEnd, bool isError)
    {
        for(auto& listener : uploadListener.getEventHandlers())
            listener.second(progress, isStart, isEnd, isError, _firmwareNodeId);
    }
}