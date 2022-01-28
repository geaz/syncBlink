#include "analyzer_mode.hpp"

namespace SyncBlink
{
    AnalyzerMode::AnalyzerMode(EventBus& eventBus, bool isAnalyzer) : _eventBus(eventBus), _isAnalyzer(isAnalyzer)
    {
    }

    void AnalyzerMode::setup()
    {
    }

    void AnalyzerMode::loop()
    {
        if (_isAnalyzer)
        {
            AudioAnalyzerResult result = _frequencyAnalyzer.loop();
            Events::AnalyzerUpdateEvent event = result.ToEvent();
            _eventBus.trigger(event);
        }
    }
}