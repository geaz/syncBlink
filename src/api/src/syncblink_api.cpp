#include "syncblink_api.hpp"

#include <iostream>
#include <mesh_connection.hpp>
#include <thread>

namespace SyncBlink
{
    namespace Api
    {
        SyncBlinkApi::SyncBlinkApi(uint64_t analyzerId, std::string analyzerName, bool setAnalyzerOnConnect)
            : _tcpClient(_messageBus), _freqAnalyzer(analyzerId, _messageBus), _analyzerId(analyzerId), _analyzerName(analyzerName),
              _setAnalyzerOnConnect(setAnalyzerOnConnect)
        {
            _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(MessageType::AnalyzerUpdate, this);
            _messageBus.addMsgHandler<Messages::AnalyzerChange>(MessageType::AnalyzerChange, this);

            _udpDiscover.errorMessageEvents.addEventHandler([this](std::string message) {
                for(auto cb : _apiOnMessageCallback) cb(message.c_str(), true);
            });
            _udpDiscover.infoMessageEvents.addEventHandler([this](std::string message) {
                for(auto cb : _apiOnMessageCallback) cb(message.c_str(), false);
            });

            _tcpClient.errorMessageEvents.addEventHandler([this](std::string message) {
                for(auto cb : _apiOnMessageCallback) cb(message.c_str(), true);
            });
            _tcpClient.infoMessageEvents.addEventHandler([this](std::string message) {
                for(auto cb : _apiOnMessageCallback) cb(message.c_str(), false);
            });
            _tcpClient.connectionEvents.addEventHandler([this](bool isConnected) {
                for(auto cb : _apiOnConnectionCallback) cb(_hubIp.c_str(), isConnected);
                if (isConnected)
                {
                    Messages::MeshConnection conmsg;
                    conmsg.nodeId = _analyzerId;
                    conmsg.isConnected = true;
                    conmsg.nodeInfo = {false, true, false, false, 0, 0, 0, 31, _analyzerName.c_str()};

                    Messages::AnalyzerChange msg;
                    msg.analyzerId = _analyzerId;

                    _tcpClient.writeMessage(conmsg.toPackage());
                    _tcpClient.writeMessage(msg.toPackage());

                    for(auto cb : _apiOnMessageCallback) cb("Connected as analyzer!", false);
                }
                else
                {
                    _hubIp = "";
                    _freqAnalyzer.stop();
                    tryConnect();
                }
            });
        }

        void SyncBlinkApi::tryConnect()
        {
            if (_connecting || _hubIp != "") return;

            _connecting = true;
            std::thread thread([this]() {
                bool first = true;
                auto lastTry = std::chrono::system_clock::now();
                while (_hubIp == "")
                {
                    auto elapsed = std::chrono::system_clock::now() - lastTry;
                    auto elapsedSecs = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

                    if (elapsedSecs > 15 || first)
                    {
                        _hubIp = _udpDiscover.ping();
                        lastTry = std::chrono::system_clock::now();
                        first = false;
                    }
                }

                if (_hubIp != "") _tcpClient.start(_hubIp);
                _connecting = false;
            });
            thread.detach();
        }

        void SyncBlinkApi::setApiOnFreqCallback(OnFreqCallback callback)
        {
            _apiOnFreqCallback.push_back(callback);
        }

        void SyncBlinkApi::setApiOnMessageCallback(OnMessageCallback callback)
        {
            _apiOnMessageCallback.push_back(callback);
        }

        void SyncBlinkApi::setApiOnConnectionCallback(OnConnectionCallback callback)
        {
            _apiOnConnectionCallback.push_back(callback);
        }

        void SyncBlinkApi::onMsg(const Messages::AnalyzerUpdate& msg)
        {
            for(auto cb : _apiOnFreqCallback) cb(msg.volume, msg.frequency);
            _tcpClient.writeMessage(msg.toPackage());
        }

        void SyncBlinkApi::onMsg(const Messages::AnalyzerChange& msg)
        {
            _activeAnalzyerId = msg.analyzerId;
            if (_activeAnalzyerId != _analyzerId) _freqAnalyzer.stop();
            else
                _freqAnalyzer.start();
        }
    }
}