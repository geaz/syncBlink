#ifndef ANALYZERMODULE_H
#define ANALYZERMODULE_H

#include "module.hpp"
#include "audio/frequency_analyzer.hpp"
#include "event/event_bus.hpp"

namespace SyncBlink
{
    class AnalyzerModule : public Module
    {
    public:
        AnalyzerModule(EventBus& eventBus);

        void loop();

    private:
        EventBus& _eventBus;
        FrequencyAnalyzer _frequencyAnalyzer;
    };
}

#endif // ANALYZERMODULE_H