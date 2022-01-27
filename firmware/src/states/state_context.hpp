#ifndef STATECONTEXT_H
#define STATECONTEXT_H

#include "event/event_bus.hpp"
#include "scripting/script_manager.hpp"
#include "state.hpp"

#include <display.hpp>
#include <led.hpp>
#include <memory>

namespace SyncBlink
{
    class StateContext
    {
    public:
        StateContext(EventBus& eventBus, LED& led, Display& display, ScriptManager& scriptManager);

        void loop();
        void resetState();
        void changeState(std::shared_ptr<State> state);

        LED& getLed();
        Display& getDisplay();
        EventBus& getEventBus();
        ScriptManager& getScriptManager();

    private:
        EventBus& _eventBus;
        LED& _led;
        Display& _display;
        ScriptManager& _scriptManager;

        std::shared_ptr<State> _currentState;
    };
}

#endif // STATECONTEXT_H