#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "module.hpp"
#include "core/event/event_bus.hpp"
#include "core/event/commands/set_display.hpp"

#include <display.hpp>

namespace SyncBlink
{
    class DisplayModule : 
        public Module,
        public EventHandler<Commands::SetDisplay>
    {
    public:
        DisplayModule(EventBus& eventBus);
        ~DisplayModule();

        void setup() override;
        void loop();

        void onEvent(const Commands::SetDisplay& command);

    private:
        Display _display;
        EventBus& _eventBus;
        uint32_t _displayCommandHandleId = 0;
    };
}

#endif // DISPLAYMODULE_H