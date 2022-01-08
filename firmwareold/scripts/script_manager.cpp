#include "script_manager.hpp"
#include "station_context.hpp"

#include <EEPROM.h>
#include <LittleFS.h>

namespace SyncBlink
{
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
        
        if(getActiveScript() == scriptName)
        {
            for(auto event : activeScriptChangedEvents.getEventHandlers()) event.second();
        }
    }

    void ScriptManager::remove(const std::string& scriptName)
    {
        LittleFS.remove(("/scripts/" + scriptName).c_str());
    }

    std::string ScriptManager::getActiveScript()
    {
        int i = 0;
        char data[SyncBlink::ScriptRomLength];
        char curChar = char(EEPROM.read(i + SyncBlink::ScriptRomStart));

        while (i < SyncBlink::ScriptRomLength && curChar != '\0')
        {
            curChar = char(EEPROM.read(i + SyncBlink::ScriptRomStart));
            data[i] = curChar;
            i++;
        }

        std::string activeScript(data);
        #ifdef DEBUG_SCRIPTMANAGER
        Serial.printf("[SCRIPTMANAGER] Active Script: %s\n", activeScript.c_str());
        #endif

        Script script = get(activeScript);
        if(!script.Exists) 
        {            
            #ifdef DEBUG_SCRIPTMANAGER
            Serial.println("Currently active script not found! Falling back ...");
            #endif

            std::vector<std::string> scriptList = getList();
            if(scriptList.size() > 0)
            {
                setActiveScript(getList().front());
                activeScript = getActiveScript();
            }
        }
        return activeScript;
    }

    void ScriptManager::setActiveScript(const std::string& scriptName)
    {
        if (scriptName.length() > 0) 
        {
            #ifdef DEBUG_SCRIPTMANAGER
            Serial.println("[ScriptManager] Clearing active script ...");
            #endif
            for (int i = SyncBlink::ScriptRomStart; i < SyncBlink::ScriptRomEnd; ++i) { EEPROM.write(i, 0); }

            #ifdef DEBUG_SCRIPTMANAGER
            Serial.printf("[ScriptManager] Saving active script (%s) ...\n", scriptName.c_str());
            #endif
            for (uint i = 0; i < scriptName.length(); ++i) EEPROM.write(i + SyncBlink::ScriptRomStart, scriptName[i]);

            EEPROM.commit();
            for(auto event : activeScriptChangedEvents.getEventHandlers()) event.second();
        }
    }
}