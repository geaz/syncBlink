#ifndef STATECONTEXT_H
#define STATECONTEXT_H

#include <memory>
#include <led.hpp>
#include <display.hpp>

#include "state.hpp"
#include "event/event_bus.hpp"
#include "scripting/script_manager.hpp"

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