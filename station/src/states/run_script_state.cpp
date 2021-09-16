#ifndef RUNSCRIPTSTATE_H
#define RUNSCRIPTSTATE_H

#include <mappings.hpp>
#include <blinkscript/blink_script.hpp>
#include <audio/frequency_analyzer.hpp>
#include <messages/message.hpp>
#include <messages/audio_analyzer_message.hpp>
#include <messages/server_messages.hpp>

#include "state.hpp"
#include "states/invalid_script_state.cpp"
#include "station_context.hpp"
#include "display/views/run_script_view.cpp"

namespace SyncBlink
{
    class RunScriptState : public State
    {
        public:
            RunScriptState(StationContext& context, std::shared_ptr<BlinkScript> blinkScript) 
                : _context(context), _blinkScript(blinkScript)
            {
                _socketEventHandleId = context.getBlinkTcpServer()
                    .messageEvents
                    .addEventHandler([this](Message message) 
                    { 
                        _newNodeConnected = message.type == Client::MESH_CONNECTION;
                        if(message.type == Client::MessageType::EXTERNAL_ANALYZER)
                            handleExternalSource(message.body);
                    });
                _scriptEventHandleId = context.getScriptManager()
                    .activeScriptChangedEvents
                    .addEventHandler([this]() 
                    {
                        _activeScriptChanged = true;
                    });
                _runScriptView = std::make_shared<RunScriptView>();
                _scriptName = _blinkScript->getScriptName();
            }

            ~RunScriptState()
            {
                _context.getBlinkTcpServer()
                    .messageEvents
                    .removeEventHandler(_socketEventHandleId);
                _context.getScriptManager()
                    .activeScriptChangedEvents
                    .removeEventHandler(_scriptEventHandleId);
            }

            void run(StationContext& context)
            {                         
                context.getDisplay().setView(_runScriptView);
                context.getDisplay().setLeftStatus(_scriptName);
                
                if(_activeScriptChanged || _newNodeConnected) context.resetState();
                else handleMicrophoneSource(context.getBlinkTcpServer());
            }

        private:
            void handleMicrophoneSource(TcpServer& tcpServer)
            {   
                if(checkBlinkScript() && _context.getNodeManager().getActiveAnalyzer() == _context.getStationId())
                {
                    AudioAnalyzerResult result = _frequencyAnalyzer.loop();
                    AudioAnalyzerMessage message = result.ToMessage();

                    uint32_t delta = millis() - _lastLedUpdate;
                    _lastLedUpdate = millis();

                    tcpServer.broadcast(&message, sizeof(message), Server::ANALYZER_UPDATE);
                    setView(message, delta);
                    _blinkScript->updateAnalyzerResult(result.volume, result.dominantFrequency);
                    _blinkScript->run(delta);
                }
            }

            void handleExternalSource(std::vector<uint8_t> message)
            {
                if(checkBlinkScript()
                && _context.getNodeManager().getActiveAnalyzer() != _context.getStationId())
                {                    
                    AudioAnalyzerMessage audioMessage;
                    memcpy(&audioMessage, &message[0], message.size());
                    
                    uint32_t delta = millis() - _lastLedUpdate;
                    _lastLedUpdate = millis();

                    _context.getBlinkTcpServer().broadcast(&audioMessage, sizeof(audioMessage), Server::ANALYZER_UPDATE);
                    setView(audioMessage, delta);

                    _blinkScript->updateAnalyzerResult(audioMessage.volume, audioMessage.frequency);
                    _blinkScript->run(delta);
                }
            }

            bool checkBlinkScript()
            {
                bool valid = true;
                if(_blinkScript->isFaulted())
                {
                    _context.currentState = std::make_shared<InvalidScriptState>(_context);
                    valid = false;
                }
                return valid;
            }

            void setView(AudioAnalyzerMessage& message, uint32_t delta)
            {
                _runScriptView->delta = delta;
                _runScriptView->volume = message.volume;
                _runScriptView->decibel = message.decibel;
                if(message.volume > 0 && message.frequency > 0)
                {
                    _runScriptView->dominantFrequency = message.frequency;
                    _runScriptView->setFreqBars(message.freqBins);
                }
                else
                {
                    _runScriptView->dominantFrequency = 0;
                    _runScriptView->fadeFrequencyRange();
                }
            }

            StationContext& _context;
            std::shared_ptr<BlinkScript> _blinkScript;
            std::shared_ptr<RunScriptView> _runScriptView;
            uint32_t _socketEventHandleId = 0, _scriptEventHandleId = 0;
            std::string _scriptName;
            bool _activeScriptChanged = false;
            bool _newNodeConnected = false;

            FrequencyAnalyzer _frequencyAnalyzer;
            uint64_t _lastLedUpdate = millis();
    };
}

#endif // RUNSCRIPTSTATE_H