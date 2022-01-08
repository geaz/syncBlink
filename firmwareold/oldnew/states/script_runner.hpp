#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

#include <memory>

#include "script.hpp"
#include "led/led.hpp"
#include <event/event_bus.hpp>
#include <event/events/script_change_event.hpp>
#include <event/events/mesh_update_event.hpp>
#include <event/events/analyzer_update_event.hpp>
#include "scripting/blinkscript/blink_script.hpp"

namespace SyncBlink
{
    class ScriptRunner : 
        public EventHandler<Events::ScriptChangeEvent>,
        public EventHandler<Events::AnalyzerUpdateEvent>
    {
        public:
            ScriptRunner(EventBus& eventBus, LED& led);
            ~ScriptRunner();

            void onEvent(const Events::ScriptChangeEvent& event);
            void onEvent(const Events::AnalyzerUpdateEvent& event);

        private:
            EventBus& _eventBus;
            LED& _led;

            uint32_t _scriptHandleId;
            uint32_t _analyzerHandleId;
            std::shared_ptr<BlinkScript> _blinkScript;
    };
}

#endif // SCRIPTRUNNER_H