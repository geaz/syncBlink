#ifndef BLINKSCRIPTMODULE_H
#define BLINKSCRIPTMODULE_H

#include "module.hpp"
#include "core/script.hpp"
#include "core/message/message_bus.hpp"
#include "core/views/icon_text_view.cpp"
#include "core/views/run_script_view.cpp"
#include "core/message/messages/script_change.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_update.hpp"

#include <string>
#include <memory>
#include <blink_script.hpp>

namespace SyncBlink
{
    class BlinkScriptModule : 
        public Module,
        public MessageHandler<Messages::AnalyzerUpdate>,
        public MessageHandler<Messages::ScriptChange>
    {
    public:
        BlinkScriptModule(LED& led, MessageBus& messageBus);
        BlinkScriptModule(LED& led, MessageBus& messageBus, Script initialScript);
        ~BlinkScriptModule();

        void loop();

        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::ScriptChange& msg);
        void onMsg(const Messages::MeshUpdate& msg);

    private:
        bool checkBlinkScript();
        void setView(Messages::AnalyzerUpdate msg, uint32_t delta);

        LED& _led;
        MessageBus& _messageBus;

        Script _currentScript;
        std::shared_ptr<BlinkScript> _blinkScript;
        bool _activeScriptChanged = false;

        std::shared_ptr<RunScriptView> _runScriptView;
        std::shared_ptr<IconTextView> _invalidScriptView;
        std::shared_ptr<IconTextView> _failSafeView;

        uint32_t _scriptHandleId = 0;
        uint32_t _analyzerHandleId = 0;

        uint64_t _lastLedUpdate = millis();
    };
}

#endif // BLINKSCRIPTMODULE_H