#ifndef BLINKSCRIPTMODULE_H
#define BLINKSCRIPTMODULE_H

#include "core/network/get_id.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/script_change.hpp"
#include "core/script.hpp"
#include "core/views/icon_text_view.cpp"
#include "core/views/run_script_view.cpp"
#include "module.hpp"

#include <blink_script.hpp>
#include <memory>
#include <string>

namespace SyncBlink
{
    class BlinkScriptModule : public Module,
                              public MessageHandler<Messages::MeshUpdate>,
                              public MessageHandler<Messages::ScriptChange>,
                              public MessageHandler<Messages::AnalyzerUpdate>
    {
    public:
        BlinkScriptModule(LED& led, MessageBus& messageBus);
        BlinkScriptModule(LED& led, MessageBus& messageBus, Script initialScript);
        ~BlinkScriptModule();

        void loop();

        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::ScriptChange& msg);
        void onMsg(const Messages::MeshUpdate& msg);

        bool getLightMode() const;
        uint64_t getActiveAnalyzer() const;

    private:
        bool checkBlinkScript();
        void setView(Messages::AnalyzerUpdate msg, uint32_t delta);

        LED& _led;
        MessageBus& _messageBus;

        Script _currentScript;
        std::shared_ptr<BlinkScript> _blinkScript;
        bool _activeScriptChanged = false;
        bool _lightMode = false;

        std::shared_ptr<RunScriptView> _runScriptView;
        std::shared_ptr<IconTextView> _invalidScriptView;
        std::shared_ptr<IconTextView> _failSafeView;

        uint32_t _meshHandleId = 0;
        uint32_t _scriptHandleId = 0;
        uint32_t _analyzerHandleId = 0;

        uint32_t _previousNodeCount = 0;
        uint32_t _previousLedCount = 0;
        uint32_t _meshLedCount = 0;

        uint64_t _lastLedUpdate = millis();
        uint64_t _activeAnalyzer = SyncBlink::getId();
    };
}

#endif // BLINKSCRIPTMODULE_H