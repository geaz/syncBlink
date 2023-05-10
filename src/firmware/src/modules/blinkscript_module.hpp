#ifndef BLINKSCRIPTMODULE_H
#define BLINKSCRIPTMODULE_H

#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/mesh_update.hpp"
#include "core/message/messages/script_load.hpp"
#include "core/network/get_id.hpp"
#include "core/script.hpp"
#include "core/script_list.hpp"
#include "module.hpp"

#include <blink_script.hpp>
#include <memory>
#include <string>

namespace SyncBlink
{
    class BlinkScriptModule : public Module,
                              public MessageHandler<Messages::MeshUpdate>,
                              public MessageHandler<Messages::ScriptLoad>,
                              public MessageHandler<Messages::AnalyzerUpdate>
    {
    public:
        BlinkScriptModule(LED& led, MessageBus& messageBus, ScriptList& scriptList, std::string nodeName, std::string nodeType);
        ~BlinkScriptModule();

        void loop() override;

        void onMsg(const Messages::AnalyzerUpdate& msg);
        void onMsg(const Messages::ScriptLoad& msg);
        void onMsg(const Messages::MeshUpdate& msg);

    private:
        void checkFailSafe();
        bool checkBlinkScript();

        LED& _led;
        MessageBus& _messageBus;
        ScriptList& _scriptList;
        std::string _nodeName;
        std::string _nodeType;

        Script _currentScript;
        BlinkScript* _blinkScript = nullptr;
        bool _activeScriptChanged = false;

        bool _inError = false;
        bool _inFailSafe = false;
        bool _failSafeChecked = false;

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