#ifndef INVALIDSCRIPTSTATE_H
#define INVALIDSCRIPTSTATE_H

#include <led.hpp>
#include <display.hpp>
#include <event_bus.hpp>

#include "state.hpp"
#include "views/icon_text_view.cpp"
#include "scripts/script_manager.hpp"
#include "fail_safe_state.cpp"

namespace SyncBlink
{
    class InvalidScriptState : public State, public EventHandler<ScriptChangedEvent>
    {
        public:
            InvalidScriptState(EventBus& eventBus, LED& led, Display& display) :
                _eventBus(eventBus),
                _led(led),
                _display(display)
            {
                _scriptEventHandleId = _eventBus.addEventHandler<ScriptChangedEvent>(this);
                _invalidScriptView = std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66);
            }

            ~InvalidScriptState()
            {
                _eventBus.removeEventHandler(_scriptEventHandleId);
            }

            void loop()
            {
                _led.setAllLeds(Colors::Red);
                _display.setView(_invalidScriptView);
                
                //if(_activeScriptChanged) context.resetState();
                _eventBus.trigger<StateChangeEvent>({ std::make_shared<FailSafeState>(_eventBus, _led, _display) });
            }

            void onEvent(const ScriptChangedEvent& event)
            {
                _activeScriptChanged = true;
            }

        private:
            EventBus& _eventBus;
            LED& _led;
            Display& _display;
            std::shared_ptr<IconTextView> _invalidScriptView;            
            uint32_t _scriptEventHandleId = 0;
            bool _activeScriptChanged = false;
    };
}

#endif // INVALIDSCRIPTSTATE_H