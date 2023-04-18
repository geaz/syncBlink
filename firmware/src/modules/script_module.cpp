#include "script_module.hpp"

#include <LittleFS.h>

namespace SyncBlink
{
    ScriptModule::ScriptModule(MessageBus& messageBus, Config& config) : _messageBus(messageBus), _config(config)
    {
    }

    Script ScriptModule::get(const std::string& scriptName)
    {
        Script script;
        script.Name = scriptName;
        script.Exists = true;

        std::string path = "/scripts/" + scriptName;
        if (LittleFS.exists(path.c_str()))
        {
            File file = LittleFS.open(path.c_str(), "r");
            while (file.available())
            {
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

    std::vector<std::string> ScriptModule::getList()
    {
        std::vector<std::string> scriptList;

        Dir dir = LittleFS.openDir("scripts");
        while (dir.next())
        {
            scriptList.push_back(dir.fileName().c_str());
        }

        return scriptList;
    }

    void ScriptModule::add(const std::string& scriptName)
    {
        File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
        file.close();
    }

    void ScriptModule::save(const std::string& scriptName, const std::string& content)
    {
        File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
        file.print(content.c_str());
        file.close();

        Script activeScript = getActiveScript();
        if (activeScript.Name == scriptName)
        {
            _messageBus.trigger<Messages::ScriptChange>({activeScript});
        }
    }

    void ScriptModule::remove(const std::string& scriptName)
    {
        Script activeScript = getActiveScript();
        LittleFS.remove(("/scripts/" + scriptName).c_str());

        if (activeScript.Name == scriptName)
        {
            activeScript = getActiveScript();
            _messageBus.trigger<Messages::ScriptChange>({activeScript});
        }
    }

    Script ScriptModule::getActiveScript()
    {
        Script script;
        const char* activeScriptName = _config.Values["active_script"];
        if (activeScriptName != nullptr)
        {
            script = get(activeScriptName);
        }

        if (!script.Exists)
        {
            Serial.println("[ScriptManager] Currently active script not found! Falling back ...");

            std::vector<std::string> scriptList = getList();
            if (scriptList.size() > 0)
            {
                setActiveScript(getList().front());
                script = getActiveScript();
            }
        }
        return script;
    }

    void ScriptModule::setActiveScript(const std::string& scriptName)
    {
        if (scriptName.length() > 0)
        {
            Serial.printf("[ScriptManager] Saving active script (%s) ...\n", scriptName.c_str());
            _config.Values["active_script"] = scriptName.c_str();
            _config.save();
            
            _messageBus.trigger<Messages::ScriptChange>(getActiveScript());
        }
    }
}