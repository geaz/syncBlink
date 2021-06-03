#ifndef INVALIDSCRIPTSTATE_H
#define INVALIDSCRIPTSTATE_H

#include "state.hpp"
#include "station_context.hpp"
#include "views/icon_text_view.cpp"

namespace SyncBlink
{
    class InvalidScriptState : public State
    {
        public:
            InvalidScriptState(StationContext& context) :
                _context(context),
                _invalidScriptView(std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66))
            {
                _scriptEventHandleId = context.getScriptManager()
                    .activeScriptChangedEvents
                    .addEventHandler([this]() 
                    {
                        _activeScriptChanged = true;
                    });
            }

            ~InvalidScriptState()
            {
                _context.getScriptManager()
                    .activeScriptChangedEvents
                    .removeEventHandler(_scriptEventHandleId);
            }

            void run(StationContext& context)
            {
                context.getLed().setAllLeds(SyncBlink::Red);
                context.getDisplay().setView(_invalidScriptView);
                
                if(_activeScriptChanged) context.resetState();
            }

        private:
            StationContext& _context;
            std::shared_ptr<IconTextView> _invalidScriptView;            
            uint32_t _scriptEventHandleId = 0;
            bool _activeScriptChanged = false;
    };
}

#endif // INVALIDSCRIPTSTATE_H