#ifndef ANALYZERMODULE_H
#define ANALYZERMODULE_H

#include "core/audio/frequency_analyzer.hpp"
#include "core/message/message_bus.hpp"
#include "module.hpp"

namespace SyncBlink
{
    class AnalyzerModule : public Module
    {
    public:
        AnalyzerModule(MessageBus& messageBus);

        void loop();

    private:
        MessageBus& _messageBus;
        FrequencyAnalyzer _frequencyAnalyzer;
    };
}

#endif // ANALYZERMODULE_H