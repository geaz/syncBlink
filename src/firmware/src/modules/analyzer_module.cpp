#include "analyzer_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    AnalyzerModule::AnalyzerModule(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(MessageType::AnalyzerChange, this);
    }

    AnalyzerModule::~AnalyzerModule()
    {
        _messageBus.removeMsgHandler(_analyzerChangeHandleId);
    }

    void AnalyzerModule::loop()
    {
        if (_activeAnalzyerId != _ownId || !_doUpdate) 
        {
            _doUpdate = true;
            return;
        }

        AudioAnalyzerResult result = _frequencyAnalyzer.loop();
        Messages::AnalyzerUpdate msg = result.ToMessage(_ownId);
        _messageBus.trigger(msg);
        
        // Just do an analyzer update every second loop.
        // We don't want to flood the network with to many packages.
        // Furthermore since platform version 3.x.x FastLED seems to have
        // problems with timings(?). Getting strange flickering or no led updates
        // at all, if updated every loop....
        _doUpdate = false;
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