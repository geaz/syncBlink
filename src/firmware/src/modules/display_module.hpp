#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "core/message/messages/script_load.hpp"
#include "core/message/messages/script_error.hpp"
#include "core/views/icon_text_view.cpp"
#include "core/views/run_script_view.cpp"
#include "module.hpp"

#include <display.hpp>

namespace SyncBlink
{
    class DisplayModule : public Module,
                          public MessageHandler<Messages::ScriptLoad>,
                          public MessageHandler<Messages::ScriptError>,
                          public MessageHandler<Messages::AnalyzerUpdate>
    {
    public:
        DisplayModule(MessageBus& messageBus);
        ~DisplayModule();

        void setup() override;
        void loop() override;

        void onMsg(const Messages::ScriptLoad& msg);
        void onMsg(const Messages::ScriptError& msg);
        void onMsg(const Messages::AnalyzerUpdate& msg);

    private:
        Display _display;
        MessageBus& _messageBus;
        uint32_t _scriptLoadHandleId = 0;
        uint32_t _scriptErrorHandleId = 0;
        uint32_t _analyzerUpdateHandleId = 0;
        uint64_t _lastUpdate = millis();

        std::shared_ptr<RunScriptView> _runScriptView;
        std::shared_ptr<IconTextView> _invalidScriptView;
    };
}

#endif // DISPLAYMODULE_H