#include "analyzer_module.hpp"

namespace SyncBlink
{
    AnalyzerModule::AnalyzerModule(EventBus& eventBus) : _eventBus(eventBus)
    { }

    void AnalyzerModule::loop()
    {
        AudioAnalyzerResult result = _frequencyAnalyzer.loop();
        Events::AnalyzerUpdateEvent event = result.ToEvent();
        _eventBus.trigger(event);
    }
}