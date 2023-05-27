#ifndef SYNCBLINKAPI_H
#define SYNCBLINKAPI_H

#include "frequency_analyzer.hpp"
#include "tcp_client.hpp"
#include "udp_discover.hpp"

#include <analyzer_change.hpp>
#include <analyzer_constants.hpp>
#include <analyzer_update.hpp>
#include <message_bus.hpp>
#include <string.h>

namespace SyncBlink
{
    namespace Api
    {
        typedef void (*OnFreqCallback)(uint8_t, uint16_t);
        typedef void (*OnConnectionCallback)(const char*, bool);
        typedef void (*OnMessageCallback)(const char*, bool);

        class SyncBlinkApi : public MessageHandler<Messages::AnalyzerUpdate>, public MessageHandler<Messages::AnalyzerChange>
        {
        public:
            SyncBlinkApi(uint64_t analyzerId, std::string analyzerName, bool setAnalyzerOnConnect);
            ~SyncBlinkApi();

            void tryConnect();

            void setApiOnFreqCallback(OnFreqCallback callback);
            void setApiOnMessageCallback(OnMessageCallback callback);
            void setApiOnConnectionCallback(OnConnectionCallback callback);

        private:
            void onMsg(const Messages::AnalyzerUpdate& msg);
            void onMsg(const Messages::AnalyzerChange& msg);

            TcpClient _tcpClient;
            UdpDiscover _udpDiscover;
            FrequencyAnalyzer _freqAnalyzer;
            MessageBus _messageBus;

            uint64_t _analyzerId;
            std::string _analyzerName;

            bool _connecting = false;
            bool _setAnalyzerOnConnect = false;

            std::string _hubIp = "";
            uint64_t _activeAnalzyerId = -1;

            std::vector<OnFreqCallback> _apiOnFreqCallback;
            std::vector<OnMessageCallback> _apiOnMessageCallback;
            std::vector<OnConnectionCallback> _apiOnConnectionCallback;
        };
    }
}

extern "C"
{
    typedef void* syncblink_api;

    __declspec(dllexport) syncblink_api syncblink_api_init(uint64_t analyzerId, const char* analyzerName, bool setAnalyzerOnConnect)
    {
        return new SyncBlink::Api::SyncBlinkApi(analyzerId, analyzerName, setAnalyzerOnConnect);
    }

    __declspec(dllexport) void syncblink_api_tryconnect(syncblink_api g)
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->tryConnect();
    }

    __declspec(dllexport) void syncblink_api_on_freq(syncblink_api g, void (*fn)(uint8_t volume, uint16_t dominant_frequency))
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->setApiOnFreqCallback(fn);
    }

    __declspec(dllexport) void syncblink_api_on_message(syncblink_api g, void (*fn)(const char* message, bool isError))
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->setApiOnMessageCallback(fn);
    }

    __declspec(dllexport) void syncblink_api_on_connection(syncblink_api g, void (*fn)(const char* hubIp, bool isConnected))
    {
        static_cast<SyncBlink::Api::SyncBlinkApi*>(g)->setApiOnConnectionCallback(fn);
    }
}

#endif // SYNCBLINKAPI_H