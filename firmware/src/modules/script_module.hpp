#ifndef SCRIPTMODULE_H
#define SCRIPTMODULE_H

#include "module.hpp"
#include "core/script.hpp"
#include "core/config/config.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/script_change.hpp"

#include <string>
#include <vector>

namespace SyncBlink
{
    class ScriptModule : public Module
    {
    public:
        ScriptModule(MessageBus& messageBus, Config& config);

        Script get(const std::string& scriptName);
        std::vector<std::string> getList();

        void add(const std::string& scriptName);
        void save(const std::string& scriptName, const std::string& content);
        void remove(const std::string& scriptName);

        Script getActiveScript();
        void setActiveScript(const std::string& scriptName);

    private:
        MessageBus& _messageBus;
        Config& _config;
    };
}

#endif // SCRIPTMODULE_H