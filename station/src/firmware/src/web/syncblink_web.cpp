#include <LittleFS.h>
#include <ArduinoJson.h>
#include <network/websocket/messages/audio_analyzer_message.hpp>

#include "syncblink_web.hpp"

namespace SyncBlink
{
    SyncBlinkWeb::SyncBlinkWeb(StationWifi& stationWifi, ModManager& modManager, NodeManager& nodeManager)
        : _server(80), _stationWifi(stationWifi), _modManager(modManager), _nodeManager(nodeManager)
    {
        _server.on("/api/mesh/info", [this]() { getMeshInfo(); });        
        _server.on("/api/mesh/flash", HTTP_POST, [this]() { _nodeManager.flash(0); }, [this]() { uploadFirmware(); yield(); });

        _server.on("/api/wifi/set", [this]() { setWifi(); });
        _server.on("/api/wifi/get", [this]() { getWifi(); });

        _server.on("/api/mods/list", [this]() { getMods(); });
        _server.on("/api/mods/get", [this]() { getModContent(); });

        _server.on("/api/mods/add", [this]() { addMod(); });
        _server.on("/api/mods/save", [this]() { saveMod(); });
        _server.on("/api/mods/delete", [this]() { deleteMod(); });

        _server.on("/api/mods/getModSettings", [this]() { getModSettings(); });
        _server.on("/api/mods/setModSettings", [this]() { setModSettings(); });
        
        _server.serveStatic("/", LittleFS, "/public/");
        _server.begin();
    }

    void SyncBlinkWeb::loop()
    {
        _server.handleClient();
    }

    void SyncBlinkWeb::getMeshInfo()
    {
        String JSON;
        DynamicJsonDocument doc(1024);
        
        auto connectedNodes = _nodeManager.getConnectedNodes();
        for(uint32_t i = 0; i < connectedNodes.size(); i++)
        {
            StaticJsonDocument<500> nodeJson;
            nodeJson["clientId"] = connectedNodes[i].clientId;
            nodeJson["parentId"] = connectedNodes[i].parentId;
            nodeJson["ledCount"] = connectedNodes[i].ledCount;
            nodeJson["firmwareVersion"] = connectedNodes[i].firmwareVersion;

            doc["nodes"][i] = nodeJson;
        }

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::setWifi()
    {
        String ssid = _server.arg("ssid");
        String pass = _server.arg("pass");
        _stationWifi.saveWifi(ssid.c_str(), pass.c_str());
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::getWifi()
    {
        std::string ssid = _stationWifi.getSavedSSID();
        std::string pass = _stationWifi.getSavedPass();

        String JSON;
        StaticJsonDocument<500> doc;
        doc["ssid"] = ssid.c_str();
        doc["pass"] = pass.c_str();
        doc["connected"] = WiFi.status() == WL_CONNECTED;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }    

    void SyncBlinkWeb::addMod()
    {
        std::string modName = _server.arg("name").c_str();

        _modManager.add(modName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::saveMod()
    {
        String body = _server.arg("plain");
        StaticJsonDocument<5000> mod;
        deserializeJson(mod, body);

        String modName = mod["name"];
        String modContent = mod["content"];

        _modManager.save(modName.c_str(), modContent.c_str());
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::deleteMod()
    {
        std::string modName = _server.arg("name").c_str();

        _modManager.remove(modName);
        _server.send(200, "application/json", "{ \"saved\": true }");
    }

    void SyncBlinkWeb::getMods()
    {
        String JSON;
        StaticJsonDocument<1000> jsonBuffer;
        JsonArray files = jsonBuffer.createNestedArray("mods");

        std::vector<std::string> modList = _modManager.getList();
        for(std::string modName : modList)
            files.add(modName.c_str());

        serializeJson(jsonBuffer, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::getModContent()
    {
        String JSON, modContent;
        StaticJsonDocument<5000> doc;    

        std::string modName = _server.arg("name").c_str();
        Mod mod = _modManager.get(modName);
        
        doc["name"] = mod.Name.c_str();
        doc["content"] = mod.Content.c_str();
        doc["exists"] = mod.Exists;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::getModSettings()
    {
        std::string activeMod = _modManager.getActiveModName();
        AudioAnalyzerSource activeSource = _modManager.getActiveSource();

        String JSON;
        StaticJsonDocument<1000> doc;
        doc["name"] = activeMod.c_str();
        doc["source"] = activeSource;

        serializeJson(doc, JSON);
        _server.send(200, "application/json", JSON);
    }

    void SyncBlinkWeb::setModSettings()
    {
        std::string modName = _server.arg("name").c_str();
        uint source = std::atoi(_server.arg("source").c_str());

        if(_modManager.get(modName).Exists && (source == 0 || source == 1)) {
            _modManager.saveActiveModName(modName);
            _modManager.saveActiveSource(static_cast<AudioAnalyzerSource>(source));
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
            _firmwareFile = LittleFS.open(FirmwarePath.c_str(), "a");
        }
        if (upload.status == UPLOAD_FILE_WRITE)
        {
            _firmwareFile.write(upload.buf, upload.currentSize);
        } 
        else if (upload.status == UPLOAD_FILE_END)
        {            
            _firmwareFile.close();
        }
        else if(upload.status == UPLOAD_FILE_ABORTED)
        {
            _firmwareFile.close();
            LittleFS.remove(FirmwarePath.c_str());
        }
    }
}