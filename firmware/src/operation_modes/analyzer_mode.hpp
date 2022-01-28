#ifndef ANALYZERMODE_H
#define ANALYZERMODE_H

#include "audio/frequency_analyzer.hpp"
#include "event/event_bus.hpp"
#include "operation_mode.hpp"

namespace SyncBlink
{
    class AnalyzerMode : public OperationMode
    {
    public:
        AnalyzerMode(EventBus& eventBus, bool isAnalyzer);

        virtual void setup();
        virtual void loop();

    private:
        bool isActiveAnalyzer();

        EventBus& _eventBus;
        bool _isAnalyzer;
        FrequencyAnalyzer _frequencyAnalyzer;
    };
}

#endif // ANALYZERMODE_H