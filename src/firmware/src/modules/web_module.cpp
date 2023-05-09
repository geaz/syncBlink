#include "web_module.hpp"

#include "core/message/messages/analyzer_change.hpp"
#include "core/message/messages/node_command.hpp"

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
        _server.on(F("/api/mesh/ping"), HTTP_GET, [this]() { pingNode(); });
        _server.on(F("/api/mesh/rename"), HTTP_GET, [this]() { renameNode(); });
        _server.on(F("/api/mesh/setNodeWifi"), HTTP_GET, [this]() { setNodeWifi(); });
        _server.on(F("/api/mesh/info"), HTTP_GET, [this]() { getMeshInfo(); });
        _server.on(F("/api/mesh/setAnalyzer"), HTTP_GET, [this]() { setAnalyzer(); });
        _server.on(F("/api/mesh/setLightMode"), HTTP_GET, [this]() { setLightMode(); });

        _server.on(F("/api/wifi/set"), [this]() { setWifi(); });
        _server.on(F("/api/wifi/get"), [this]() { getWifi(); });

        _server.on(F("/api/scripts/list"), HTTP_GET, [this]() { getScriptList(); });
        _server.on(F("/api/scripts/get"), HTTP_GET, [this]() { getScriptContent(); });

        _server.on(F("/api/scripts/add"), HTTP_GET, [this]() { addScript(); });
        _server.on(
            F("/api/scripts/save"), HTTP_POST, [this]() { _server.send(200, F("text/plain"), ""); }, [this]() { saveScript(false); });
        _server.on(
            F("/api/scripts/savebc"), HTTP_POST, [this]() { _server.send(200, F("text/plain"), ""); }, [this]() { saveScript(true); });
        _server.on(F("/api/scripts/delete"), HTTP_GET, [this]() { deleteScript(); });
        _server.on(F("/api/scripts/set"), HTTP_GET, [this]() { setActiveScript(); });

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

        _messageBus.trigger(Messages::AnalyzerChange{analyzerId});
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
        std::string ssid = _config.Values[F("wifi_ssid")];
        auto activeScriptName = _scriptModule.getActiveScript().Name;
        auto stationInfo = _wifiModule.getStationInfo();
        auto connectedNodes = _wifiModule.getConnectedNodes();
        connectedNodes[std::get<0>(stationInfo)] = std::get<1>(stationInfo);

        if (!_server.chunkedResponseModeStart(200, "application/json"))
        {
            _server.send(505, F("text/html"), F("HTTP1.1 required"));
            return;
        }
        _server.sendContent("{\"nodes\":[");

        auto iter = connectedNodes.begin();
        auto endIter = connectedNodes.end();
        for (; iter != endIter;)
        {
            auto node = iter->second;
            _server.sendContent(("{\"isStation\":" + std::string(node.isStation ? "true" : "false") + ",").c_str());
            _server.sendContent(("\"isAnalyzer\":" + std::string(node.isAnalyzer ? "true" : "false") + ",").c_str());
            _server.sendContent(("\"isNode\":" + std::string(node.isNode ? "true" : "false") + ",").c_str());
            _server.sendContent(("\"nodeId\":" + toString(iter->first) + ",").c_str());
            _server.sendContent(("\"parentId\":" + toString(node.parentId) + ",").c_str());
            _server.sendContent(("\"ledCount\":" + toString(node.ledCount) + ",").c_str());
            _server.sendContent(("\"majorVersion\":" + toString(node.majorVersion) + ",").c_str());
            _server.sendContent(("\"minorVersion\":" + toString(node.minorVersion) + ",").c_str());
            _server.sendContent(("\"label\":\"" + node.nodeLabel + "\",").c_str());

            ++iter;
            if (iter == endIter)
                _server.sendContent(
                    ("\"connectedToMeshWifi\":" + std::string(node.connectedToMeshWifi ? "true" : "false") + "}],").c_str());
            else
                _server.sendContent(("\"connectedToMeshWifi\":" + std::string(node.connectedToMeshWifi ? "true" : "false") + "},").c_str());
        }

        _server.sendContent(("\"analyzer\":" + toString(_analyzerModule.getActiveAnalyzer()) + ",").c_str());
        _server.sendContent(("\"lightMode\":" + std::string(_blinkScriptModule.getLightMode() ? "true" : "false") + ",").c_str());
        _server.sendContent(("\"ssid\":\"" + ssid + "\",").c_str());
        _server.sendContent(("\"connected\":" + std::string(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",").c_str());
        _server.sendContent(("\"script\":\"" + activeScriptName + "\"}").c_str());
        _server.chunkedResponseFinalize();
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

        if (!_server.chunkedResponseModeStart(200, "application/json"))
        {
            _server.send(505, F("text/html"), F("HTTP1.1 required"));
            return;
        }
        _server.sendContent(("{\"ssid\":\"" + ssid + "\",").c_str());
        _server.sendContent(("\"connected\":" + std::string(WiFi.status() == WL_CONNECTED ? "true" : "false") + "}").c_str());
        _server.chunkedResponseFinalize();
    }

    void WebModule::addScript()
    {
        std::string scriptName = _server.arg(F("name")).c_str();

        _scriptModule.add(scriptName);
        _server.send(200, F("application/json"), "{ \"saved\": true }");
    }

    void WebModule::saveScript(bool isBytecode)
    {
        HTTPUpload& upload = _server.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
            _script = _scriptModule.get(upload.filename.c_str());
            if (!_script.Exists)
            {
                _server.send(500, F("text/plain"), F("Upload Failed! Script does not exist!"));
                return;
            }
        }
        else if (upload.status == UPLOAD_FILE_WRITE && _script.Exists)
        {
            size_t bytesWritten = 0;
            if (isBytecode) bytesWritten = _script.getBytecodeFile(true).write(upload.buf, upload.currentSize);
            else
                bytesWritten = _script.getScriptFile(true).write(upload.buf, upload.currentSize);

            if (bytesWritten != upload.currentSize)
            {
                _server.send(500, F("text/plain"), F("Write failed!"));
                return;
            }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
            _script.closeFile();
            if (isBytecode && _scriptModule.getActiveScript().Name == _script.Name)
                _messageBus.trigger(Messages::ScriptChange{_script.Name});
        }
        else
            _server.send(500, F("text/plain"), F("Upload failed!"));
    }

    void WebModule::deleteScript()
    {
        std::string scriptName = _server.arg(F("name")).c_str();

        _scriptModule.remove(scriptName);
        _server.send(200, F("application/json"), F("{ \"saved\": true }"));
    }

    void WebModule::getScriptList()
    {
        if (!_server.chunkedResponseModeStart(200, "application/json"))
        {
            _server.send(505, F("text/html"), F("HTTP1.1 required"));
            return;
        }

        std::vector<Script> scriptList = _scriptModule.getList();
        _server.sendContent("{\"scripts\":[");

        auto iter = scriptList.begin();
        auto endIter = scriptList.end();
        for (; iter != endIter;)
        {
            auto script = iter;
            _server.sendContent(("{\"name\":\"" + std::string(script->Name) + "\",").c_str());

            ++iter;
            if (iter == endIter)
                _server.sendContent(("\"isCompiled\":" + std::string(script->IsCompiled ? "true" : "false") + "}]}").c_str());
            else
                _server.sendContent(("\"isCompiled\":" + std::string(script->IsCompiled ? "true" : "false") + "},").c_str());
        }
        _server.chunkedResponseFinalize();
    }

    void WebModule::getScriptContent()
    {
        std::string scriptName = _server.arg(F("name")).c_str();
        Script script = _scriptModule.get(scriptName);
        _server.streamFile(script.getScriptFile(), "text/html");
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