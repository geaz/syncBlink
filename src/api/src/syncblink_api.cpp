#include "syncblink_api.hpp"

#include <iostream>
#include <mesh_connection.hpp>

namespace SyncBlink
{
    namespace Api
    {
        SyncBlinkApi::SyncBlinkApi(std::string url, uint64_t analyzerId, std::string analyzerName) 
            : _tcpClient(url, _messageBus), _freqAnalyzer(analyzerId, _messageBus), _analyzerId(analyzerId), _analyzerName(analyzerName)
        { 
            _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(this);
            _analyzerChangeHandleId = _messageBus.addMsgHandler<Messages::AnalyzerChange>(this);
        }

        SyncBlinkApi::~SyncBlinkApi()
        {
            _messageBus.removeMsgHandler(_analyzerHandleId);
            _messageBus.removeMsgHandler(_analyzerChangeHandleId);
        }

        void SyncBlinkApi::start()
        {
            Messages::MeshConnection msg;
            msg.nodeId = _analyzerId;
            msg.isConnected = true;
            msg.nodeInfo = { false, true, false, false, 0, 0, 0, 21, _analyzerName.c_str() };

            _freqAnalyzer.start();
            _tcpClient.start(msg.toPackage());
        }

        void SyncBlinkApi::stop()
        {
            _tcpClient.stop();
            _freqAnalyzer.stop();
        }

        void SyncBlinkApi::setApiOnFreqCallback(OnFreqCallback callback)
        {
            _apiOnFreqCallback = callback;
        }

        void SyncBlinkApi::onMsg(const Messages::AnalyzerUpdate& msg)
        {
            if(_activeAnalzyerId != _analyzerId) return;
            
            if(_apiOnFreqCallback != nullptr) _apiOnFreqCallback(msg.volume, msg.frequency);
            _tcpClient.writeMessage(msg.toPackage());
        }

        void SyncBlinkApi::onMsg(const Messages::AnalyzerChange& msg)
        {
            _activeAnalzyerId = msg.analyzerId;
        }
    }
}