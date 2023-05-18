#ifndef ANALYZERMODULE_H
#define ANALYZERMODULE_H

#include "core/audio/frequency_analyzer.hpp"
#include "core/message/message_bus.hpp"
#include "core/message/messages/analyzer_change.hpp"
#include "core/network/get_id.hpp"
#include "core/mesh_info.hpp"
#include "module.hpp"

namespace SyncBlink
{
    // Just skip the analyzer every other loop to take the pressure
    // out of the network mesh
    static const int8_t UpdateTimeout = 15;

    class AnalyzerModule : public Module, public MessageHandler<Messages::AnalyzerChange>
    {
    public:
        AnalyzerModule(MessageBus& messageBus, MeshInfo& meshInfo);
        ~AnalyzerModule();

        void loop() override;

        void onMsg(const Messages::AnalyzerChange& msg);

    private:
        MessageBus& _messageBus;
        MeshInfo& _meshInfo;
        FrequencyAnalyzer _frequencyAnalyzer;

        long _lastUpdate = -1;
        uint32_t _analyzerChangeHandleId = 0;
        uint64_t _ownId = SyncBlink::getId();
    };
}

#endif // ANALYZERMODULE_H