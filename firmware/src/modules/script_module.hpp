#ifndef SCRIPTMODULE_H
#define SCRIPTMODULE_H

#include "module.hpp"
#include "event/event_bus.hpp"
#include "scripting/script.hpp"
#include "views/icon_text_view.cpp"
#include "views/run_script_view.cpp"
#include "event/events/script_change_event.hpp"
#include "event/events/analyzer_update_event.hpp"

#include <string>
#include <memory>
#include <blink_script.hpp>

namespace SyncBlink
{
    class ScriptModule : 
        public Module,
        public EventHandler<Events::AnalyzerUpdateEvent>,
        public EventHandler<Events::ScriptChangeEvent>
    {
    public:
        ScriptModule(LED& led, EventBus& eventBus, Script initialScript);
        ~ScriptModule();

        void loop();

        void onEvent(const Events::AnalyzerUpdateEvent& event);
        void onEvent(const Events::ScriptChangeEvent& event);

    private:
        bool checkBlinkScript();
        void setView(Events::AnalyzerUpdateEvent event, uint32_t delta);

        LED& _led;
        EventBus& _eventBus;

        Script _currentScript;
        std::shared_ptr<BlinkScript> _blinkScript;
        bool _activeScriptChanged = false;

        std::shared_ptr<RunScriptView> _runScriptView;
        std::shared_ptr<IconTextView> _invalidScriptView;
        std::shared_ptr<IconTextView> _failSafeView;

        uint32_t _scriptEventHandleId = 0;
        uint32_t _analyzerEventHandleId = 0;

        uint64_t _lastLedUpdate = millis();
    };
}

#endif // SCRIPTMODULE_H