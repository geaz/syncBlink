#ifndef FAILSAFESTATE_H
#define FAILSAFESTATE_H

#include "event/events/script_change_event.hpp"
#include "state.hpp"
#include "state_context.hpp"
#include "views/icon_text_view.cpp"

#include <display.hpp>
#include <led.hpp>

namespace SyncBlink
{
    class FailSafeState : public State, public EventHandler<Events::ScriptChangeEvent>
    {
    public:
        FailSafeState(StateContext& context) : _context(context)
        {
            _scriptEventHandleId = _context.getEventBus().addEventHandler<Events::ScriptChangeEvent>(this);
            _failSafeView = std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78);
        }

        ~FailSafeState()
        {
            _context.getEventBus().removeEventHandler(_scriptEventHandleId);
        }

        void loop()
        {
            _context.getLed().setAllLeds(Colors::Yellow);
            _context.getDisplay().setView(_failSafeView);

            if (_activeScriptChanged) _context.resetState();
        }

        void onEvent(const Events::ScriptChangeEvent& event)
        {
            _activeScriptChanged = true;
        }

    private:
        StateContext& _context;
        std::shared_ptr<IconTextView> _failSafeView;
        uint32_t _scriptEventHandleId = 0;
        bool _activeScriptChanged = false;
    };
}

#endif // FAILSAFESTATE_H