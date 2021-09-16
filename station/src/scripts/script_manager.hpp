#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include "script.hpp"

#include <vector>
#include <string>
#include <functional>
#include <messages/audio_analyzer_message.hpp>
#include <event_registration.hpp>

namespace SyncBlink
{
    typedef std::function<void()> ActiveScriptChangedEvent;

    class ScriptManager
    {
        public:
            Script get(const std::string& scriptName);
            std::vector<std::string> getList();
            
            void add(const std::string& scriptName);
            void save(const std::string& scriptName, const std::string& content);
            void remove(const std::string& scriptName);

            std::string getActiveScript();
            void setActiveScript(const std::string& scriptName);

            EventRegistration<ActiveScriptChangedEvent> activeScriptChangedEvents;
    };
}

#endif // SCRIPTMANAGER_H