#ifndef SCRIPTMODULE_H
#define SCRIPTMODULE_H

#include "module.hpp"
#include "core/script.hpp"
#include "core/config/config.hpp"
#include "core/event/event_bus.hpp"
#include "core/event/events/script_change_event.hpp"

#include <string>
#include <vector>

namespace SyncBlink
{
    class ScriptModule : public Module
    {
    public:
        ScriptModule(EventBus& eventBus, Config& config);

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

#endif // SCRIPTMODULE_H