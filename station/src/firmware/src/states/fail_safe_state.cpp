#ifndef FAILSAFESTATE_H
#define FAILSAFESTATE_H

#include "state.hpp"
#include "../station_context.hpp"
#include "../views/icon_text_view.cpp"

namespace SyncBlink
{
    class FailSafeState : public State
    {
        public:
            FailSafeState(StationContext& context) :
                _context(context),
                _failSafeView(std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78))
            {
                _modEventHandleId = context.getModManager()
                    .activeModChangedEvents
                    .addEventHandler([this]() 
                    {
                        _activeModChanged = true;
                    });
            }

            ~FailSafeState()
            {
                _context.getModManager()
                    .activeModChangedEvents
                    .removeEventHandler(_modEventHandleId);
            }

            void run(StationContext& context)
            {
                context.getLed().setAllLeds(SyncBlink::Yellow);
                context.getDisplay().setView(_failSafeView);
                
                if(_activeModChanged) context.resetState();
            }

        private:
            StationContext& _context;
            std::shared_ptr<IconTextView> _failSafeView;            
            uint64_t _modEventHandleId = 0;
            bool _activeModChanged = false;
    };
}

#endif // FAILSAFESTATE_H