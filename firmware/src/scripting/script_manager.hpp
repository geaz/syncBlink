#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include "config/config.hpp"
#include "event/event_bus.hpp"
#include "event/events/script_change_event.hpp"
#include "script.hpp"

#include <functional>
#include <string>
#include <vector>

namespace SyncBlink
{
    class ScriptManager
    {
    public:
        ScriptManager(EventBus& eventBus, Config& config);

        Script get(const std::string& scriptName);
        std::vector<std::string> getList();

        void add(const std::string& scriptName);
        void save(const std::string& scriptName, const std::string& content);
        void remove(const std::string& scriptName);

        Script getActiveScript();
        void setActiveScript(const std::string& scriptName);

    private:
        EventBus& _eventBus;
        Config& _config;
    };
}

#endif // SCRIPTMANAGER_H