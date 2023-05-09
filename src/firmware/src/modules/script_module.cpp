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
        script.ScriptPath = "/scripts/" + scriptName;
        script.BytecodePath = "/bytecodes/" + scriptName;
        script.Exists = LittleFS.exists(script.ScriptPath.c_str());
        script.IsCompiled = LittleFS.exists(script.BytecodePath.c_str());

        return script;
    }

    std::vector<Script> ScriptModule::getList()
    {
        std::vector<Script> scriptList;

        Dir dir = LittleFS.openDir(F("scripts"));
        while (dir.next())
        {
            scriptList.push_back(get(dir.fileName().c_str()));
        }

        return scriptList;
    }

    void ScriptModule::add(const std::string& scriptName)
    {
        File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
        file.close();
    }

    void ScriptModule::remove(const std::string& scriptName)
    {
        Script activeScript = getActiveScript();
        LittleFS.remove(("/scripts/" + scriptName).c_str());

        if (activeScript.Name == scriptName)
        {
            activeScript = getActiveScript();
            _messageBus.trigger(Messages::ScriptChange{activeScript.Name});
        }
    }

    Script ScriptModule::getActiveScript()
    {
        Script script;
        const char* activeScriptName = _config.Values[F("active_script")];
        if (activeScriptName != nullptr)
        {
            script = get(activeScriptName);
        }

        if (!script.IsCompiled)
        {
            Serial.println(F("[ScriptManager] Currently active script not found! Falling back ..."));

            std::vector<Script> scriptList = getList();
            for(size_t i = 0; i < scriptList.size(); i++) 
            {
                Script script = scriptList[i];
                if(script.IsCompiled) 
                {
                    setActiveScript(script.Name);
                    break;
                }
            }
        }
        return script;
    }

    void ScriptModule::setActiveScript(const std::string& scriptName)
    {
        if (scriptName.length() > 0)
        {
            Serial.printf_P(PSTR("[ScriptManager] Saving active script (%s) ...\n"), scriptName.c_str());
            _config.Values[F("active_script")] = scriptName.c_str();
            _config.save();

            _messageBus.trigger(Messages::ScriptChange{getActiveScript().Name});
        }
    }
}