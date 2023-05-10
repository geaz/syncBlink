#ifndef SCRIPTLIST_H
#define SCRIPTLIST_H

#include "core/script.hpp"

#include <string>
#include <vector>
#include <LittleFS.h>

namespace SyncBlink
{
    class ScriptList
    {
    public:
        Script get(const std::string& scriptName)
        {
            Script script;
            script.Name = scriptName;
            script.ScriptPath = "/scripts/" + scriptName;
            script.BytecodePath = "/bytecodes/" + scriptName;
            script.Exists = LittleFS.exists(script.ScriptPath.c_str());
            script.IsCompiled = LittleFS.exists(script.BytecodePath.c_str());

            return script;
        }

        std::vector<Script> getList()
        {
            std::vector<Script> scriptList;

            Dir dir = LittleFS.openDir(F("scripts"));
            while (dir.next())
            {
                scriptList.push_back(get(dir.fileName().c_str()));
            }

            return scriptList;
        }

        void add(const std::string& scriptName)
        {
            File file = LittleFS.open(("/scripts/" + scriptName).c_str(), "w");
            file.close();
        }

        void remove(const std::string& scriptName)
        {            
            LittleFS.remove(("/scripts/" + scriptName).c_str());
        }
    };
}

#endif // SCRIPTLIST_H