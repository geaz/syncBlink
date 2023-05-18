#ifndef SYNCBLINKAPI_H
#define SYNCBLINKAPI_H

#include "tcp_client.hpp"
#include "frequency_analyzer.hpp"

#include <string.h>
#include <message_bus.hpp>
#include <analyzer_constants.hpp>
#include <analyzer_update.hpp>
#include <analyzer_change.hpp>

namespace SyncBlink
{
    namespace Api
    {
        typedef void (*OnFreqCallback)(uint8_t, uint16_t);

        class SyncBlinkApi : public MessageHandler<Messages::AnalyzerUpdate>,
                             public MessageHandler<Messages::AnalyzerChange>
        {
            public:
                SyncBlinkApi(std::string url, uint64_t analyzerId, std::string analyzerName);
                ~SyncBlinkApi();

                void start();
                void stop();
                void setApiOnFreqCallback(OnFreqCallback callback);

            private:
                void onMsg(const Messages::AnalyzerUpdate& msg);
                void onMsg(const Messages::AnalyzerChange& msg);

                TcpClient _tcpClient;

                uint64_t _analyzerId;
                std::string _analyzerName;
                FrequencyAnalyzer _freqAnalyzer;
                MessageBus _messageBus;

                uint64_t _activeAnalzyerId = -1;
                uint32_t _analyzerHandleId = 0;
                uint32_t _analyzerChangeHandleId = 0;
                OnFreqCallback _apiOnFreqCallback = nullptr;
        };
    }
}

extern "C" {
    typedef void* syncblink_api;

    __declspec(dllexport) syncblink_api syncblink_api_init(const char* url, uint64_t analyzerId, const char* analyzerName)
    {
        return new SyncBlink::Api::SyncBlinkApi(url, analyzerId, analyzerName);
    }

    __declspec(dllexport) void syncblink_api_start(syncblink_api g)
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->start();
    }

    __declspec(dllexport) void syncblink_api_stop(syncblink_api g)
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->stop();
    }

    __declspec(dllexport) void syncblink_api_on_freq(
        syncblink_api g,
        void (*fn)(uint8_t volume, uint16_t dominant_frequency))
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->setApiOnFreqCallback(fn);
    }
}

#endif // SyncBlinkDESKTOP_H