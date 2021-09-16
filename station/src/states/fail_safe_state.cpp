#ifndef FAILSAFESTATE_H
#define FAILSAFESTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "display/views/icon_text_view.cpp"

namespace SyncBlink
{
    class FailSafeState : public State
    {
        public:
            FailSafeState(StationContext& context) :
                _context(context),
                _failSafeView(std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78))
            {
                _scriptEventHandleId = context.getScriptManager()
                    .activeScriptChangedEvents
                    .addEventHandler([this]() 
                    {
                        _activeScriptChanged = true;
                    });
            }

            ~FailSafeState()
            {
                _context.getScriptManager()
                    .activeScriptChangedEvents
                    .removeEventHandler(_scriptEventHandleId);
            }

            void run(StationContext& context)
            {
                context.getLed().setAllLeds(SyncBlink::Yellow);
                context.getDisplay().setView(_failSafeView);
                
                if(_activeScriptChanged) context.resetState();
            }

        private:
            StationContext& _context;
            std::shared_ptr<IconTextView> _failSafeView;            
            uint32_t _scriptEventHandleId = 0;
            bool _activeScriptChanged = false;
    };
}

#endif // FAILSAFESTATE_H