#include "analyzer_module.hpp"

#include "core/network/get_id.hpp"

namespace SyncBlink
{
    AnalyzerModule::AnalyzerModule(MessageBus& messageBus, MeshInfo& meshInfo) : _messageBus(messageBus), _meshInfo(meshInfo)
    {
        _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(MessageType::AnalyzerChange, this);
    }

    AnalyzerModule::~AnalyzerModule()
    {
        _messageBus.removeMsgHandler(_analyzerChangeHandleId);
    }

    void AnalyzerModule::loop()
    {
        if (_meshInfo.getActiveAnalyzer() != _ownId || millis() - _lastUpdate < UpdateTimeout) return;

        AudioAnalyzerResult result = _frequencyAnalyzer.loop();
        Messages::AnalyzerUpdate msg = result.ToMessage(_ownId);
        _messageBus.trigger(msg);
        _lastUpdate = millis();
    }

    void AnalyzerModule::onMsg(const Messages::AnalyzerChange& msg)
    {
        _meshInfo.setActiveAnalyzer(msg.analyzerId);
    }
}