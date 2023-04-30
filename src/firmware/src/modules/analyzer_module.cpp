#include "analyzer_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    AnalyzerModule::AnalyzerModule(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(this);
    }

    AnalyzerModule::~AnalyzerModule()
    {
        _messageBus.removeMsgHandler(_analyzerChangeHandleId);
    }

    void AnalyzerModule::loop()
    {
        if(_activeAnalzyerId != _ownId || millis() - _lastUpdate < UpdateTimeout) return;
        
        AudioAnalyzerResult result = _frequencyAnalyzer.loop();
        Messages::AnalyzerUpdate msg = result.ToMessage(_ownId);
        _messageBus.trigger(msg);
        _lastUpdate = millis();
    }

    void AnalyzerModule::onMsg(const Messages::AnalyzerChange& msg)
    {
        _activeAnalzyerId = msg.analyzerId;
    }
    
    uint64_t AnalyzerModule::getActiveAnalyzer() const
    {
        return _activeAnalzyerId;
    }
}