#ifndef INVALIDSCRIPTSTATE_H
#define INVALIDSCRIPTSTATE_H

#include <led.hpp>
#include <display.hpp>

#include "state.hpp"
#include "state_context.hpp"
#include "event/event_bus.hpp"
#include "event/events/script_change_event.hpp"
#include "views/icon_text_view.cpp"
#include "scripting/script_manager.hpp"
#include "fail_safe_state.cpp"

namespace SyncBlink
{
    class InvalidScriptState : public State, public EventHandler<Events::ScriptChangeEvent>
    {
        public:
            InvalidScriptState(StateContext& context) : _context(context)
            {
                _scriptEventHandleId = _context.getEventBus().addEventHandler<Events::ScriptChangeEvent>(this);
                _invalidScriptView = std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66);
            }

            ~InvalidScriptState()
            {
                _context.getEventBus().removeEventHandler(_scriptEventHandleId);
            }

            void loop()
            {
                _context.getLed().setAllLeds(Colors::Red);
                _context.getDisplay().setView(_invalidScriptView);
                
                if(_activeScriptChanged) _context.resetState();
                _context.changeState(std::make_shared<FailSafeState>(_context));
            }

            void onEvent(const Events::ScriptChangeEvent& event)
            {
                _activeScriptChanged = true;
            }

        private:
            StateContext& _context;
            std::shared_ptr<IconTextView> _invalidScriptView;            
            uint32_t _scriptEventHandleId = 0;
            bool _activeScriptChanged = false;
    };
}

#endif // INVALIDSCRIPTSTATE_H