#ifndef BLINKSCRIPTMODULE_H
#define BLINKSCRIPTMODULE_H

#include "module.hpp"
#include "core/script.hpp"
#include "core/event/event_bus.hpp"
#include "core/views/icon_text_view.cpp"
#include "core/views/run_script_view.cpp"
#include "core/event/events/script_change_event.hpp"
#include "core/event/events/analyzer_update_event.hpp"
#include "core/event/events/mesh_update_event.hpp"

#include <string>
#include <memory>
#include <blink_script.hpp>

namespace SyncBlink
{
    class BlinkScriptModule : 
        public Module,
        public EventHandler<Events::AnalyzerUpdateEvent>,
        public EventHandler<Events::ScriptChangeEvent>
    {
    public:
        BlinkScriptModule(LED& led, EventBus& eventBus);
        BlinkScriptModule(LED& led, EventBus& eventBus, Script initialScript);
        ~BlinkScriptModule();

        void loop();

        void onEvent(const Events::AnalyzerUpdateEvent& event);
        void onEvent(const Events::ScriptChangeEvent& event);
        void onEvent(const Events::MeshUpdateEvent& event);

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

#endif // BLINKSCRIPTMODULE_H