#ifndef ANALYZERMODULE_H
#define ANALYZERMODULE_H

#include "core/audio/frequency_analyzer.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_change.hpp"
#include "core/network/get_id.hpp"
#include "module.hpp"

namespace SyncBlink
{
    class AnalyzerModule : public Module, public MessageHandler<Messages::AnalyzerChange>
    {
    public:
        AnalyzerModule(MessageBus& messageBus);
        ~AnalyzerModule();

        void loop();

        void onMsg(const Messages::AnalyzerChange& msg);

        uint64_t getActiveAnalyzer() const;

    private:
        MessageBus& _messageBus;
        FrequencyAnalyzer _frequencyAnalyzer;

        bool _doUpdate = true;
        uint64_t _activeAnalzyerId;
        uint32_t _analyzerChangeHandleId = 0;
        uint64_t _ownId = SyncBlink::getId();
    };
}

#endif // ANALYZERMODULE_H