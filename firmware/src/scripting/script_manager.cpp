#include "script_manager.hpp"

#include <EEPROM.h>
#include <LittleFS.h>

namespace SyncBlink
{
    ScriptManager::ScriptManager(EventBus& eventBus, Config& config) : _eventBus(eventBus), _config(config)
    { 
        _eventBus.trigger<Events::ScriptChangeEvent>({ getActiveScript() });
    }

    Script ScriptManager::get(const std::string& scriptName)
    {
        Script script;
        script.Name = scriptName;
        script.Exists = true;

        std::string path = "/scripts/" + scriptName;
        if(LittleFS.exists(path.c_str()))
        {
            File file = LittleFS.open(path.c_str(), "r");
            while (file.available()) {
                script.Content = file.readString().c_str();
            }
            file.close();
        }
        else
        {
            script.Exists = false;
        }        

        return script;
    }

    std::vector<std::string> ScriptManager::getList()
    {
        std::vector<std::string> scriptList;

        Dir dir = LittleFS.openDir("scripts");
        while (dir.next())
        {
            scriptList.push_back(dir.fileName().c_str());
        }

        return scriptList;
    }

    void ScriptManager::add(const std::string& scriptName)
    {
        File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
        file.close();
    }

    void ScriptManager::save(const std::string& scriptName, const std::string& content)
    {
        File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
        file.print(content.c_str());
        file.close();
        
        Script activeScript = getActiveScript();
        if(activeScript.Name == scriptName)
        {
            _eventBus.trigger<Events::ScriptChangeEvent>({ activeScript });
        }
    }

    void ScriptManager::remove(const std::string& scriptName)
    {
        Script activeScript = getActiveScript();
        LittleFS.remove(("/scripts/" + scriptName).c_str());

        if(activeScript.Name == scriptName)
        {
            activeScript = getActiveScript();
            _eventBus.trigger<Events::ScriptChangeEvent>({ activeScript });
        }
    }

    Script ScriptManager::getActiveScript()
    {
        Script script;
        const char* activeScriptName = _config.Values["active_script"];
        if(activeScriptName != nullptr)
        {
            Serial.printf("[SCRIPTMANAGER] Active Script: %s\n", activeScriptName);
            script = get(activeScriptName);
        }

        if(!script.Exists)
        {            
            Serial.println("[SCRIPTMANAGER] Currently active script not found! Falling back ...");

            std::vector<std::string> scriptList = getList();
            if(scriptList.size() > 0)
            {
                setActiveScript(getList().front());
                script = getActiveScript();
            }
        }
        return script;
    }

    void ScriptManager::setActiveScript(const std::string& scriptName)
    {
        if (scriptName.length() > 0) 
        {
            Serial.printf("[ScriptManager] Saving active script (%s) ...\n", scriptName.c_str());
            _config.Values["active_script"] = scriptName.c_str();
            _config.save();

            _eventBus.trigger<Events::ScriptChangeEvent>({ getActiveScript() });
        }
    }
}