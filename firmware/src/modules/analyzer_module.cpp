#include "analyzer_module.hpp"

namespace SyncBlink
{
    AnalyzerModule::AnalyzerModule(MessageBus& messageBus) : _messageBus(messageBus)
    { }

    void AnalyzerModule::loop()
    {
        AudioAnalyzerResult result = _frequencyAnalyzer.loop();
        Messages::AnalyzerUpdate msg = result.ToMessage();
        _messageBus.trigger(msg);
    }
}