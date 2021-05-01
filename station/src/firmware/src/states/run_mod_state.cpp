#ifndef RUNMODSTATE_H
#define RUNMODSTATE_H

#include <mappings.hpp>
#include <blinkscript/blink_script.hpp>
#include <audio/frequency_analyzer.hpp>
#include <network/tcp/messages/audio_analyzer_message.hpp>
#include <network/tcp/messages/server_messages.hpp>

#include "state.hpp"
#include "states/invalid_mod_state.cpp"
#include "station_context.hpp"
#include "views/run_mod_view.cpp"

namespace SyncBlink
{
    class RunModState : public State
    {
        public:
            RunModState(StationContext& context, std::shared_ptr<BlinkScript> blinkScript) 
                : _context(context), _blinkScript(blinkScript)
            {
                _socketEventHandleId = context.getTcpServer()
                    .messageEvents
                    .addEventHandler([this](TcpMessage message) 
                    { 
                        _newNodeConnected = message.messageType == Client::MESH_CONNECTION;
                        if(message.messageType == Client::MessageType::EXTERNAL_ANALYZER)
                            handleExternalSource(message.message);
                    });
                _modEventHandleId = context.getModManager()
                    .activeModChangedEvents
                    .addEventHandler([this]() 
                    {
                        _activeModChanged = true;
                    });
                _runModView = std::make_shared<RunModView>();
                _modName = _blinkScript->getModName();
            }

            ~RunModState()
            {
                _context.getTcpServer()
                    .messageEvents
                    .removeEventHandler(_socketEventHandleId);
                _context.getModManager()
                    .activeModChangedEvents
                    .removeEventHandler(_modEventHandleId);
            }

            void run(StationContext& context)
            {                         
                context.getDisplay().setView(_runModView);
                context.getDisplay().setLeftStatus(_modName);
                
                if(_activeModChanged || _newNodeConnected) context.resetState();
                else handleMicrophoneSource(context.getTcpServer());
            }

        private:
            void handleMicrophoneSource(TcpServer& tcpServer)
            {   
                if(checkBlinkScript() && _context.getModManager().getActiveSource() == AudioAnalyzerSource::Station)
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
                && _context.getModManager().getActiveSource() != AudioAnalyzerSource::Station)
                {                    
                    AudioAnalyzerMessage audioMessage;
                    memcpy(&audioMessage, &message[0], message.size());

                    uint32_t delta = millis() - _lastLedUpdate;
                    _lastLedUpdate = millis();

                    _context.getTcpServer().broadcast(&audioMessage, sizeof(audioMessage), Server::ANALYZER_UPDATE);
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
                    _context.currentState = std::make_shared<InvalidModState>(_context);
                    valid = false;
                }
                return valid;
            }

            void setView(AudioAnalyzerMessage& message, uint32_t delta)
            {
                _runModView->delta = delta;
                _runModView->volume = message.volume;
                _runModView->decibel = message.decibel;
                if(message.volume > 0 && message.frequency > 0)
                {
                    _runModView->dominantFrequency = message.frequency;
                    _runModView->setFreqBars(message.freqBins);
                }
                else
                {
                    _runModView->dominantFrequency = 0;
                    _runModView->fadeFrequencyRange();
                }
            }

            StationContext& _context;
            std::shared_ptr<BlinkScript> _blinkScript;
            std::shared_ptr<RunModView> _runModView;
            uint64_t _socketEventHandleId = 0, _modEventHandleId = 0;
            std::string _modName;
            bool _activeModChanged = false;
            bool _newNodeConnected = false;

            FrequencyAnalyzer _frequencyAnalyzer;
            uint64_t _lastLedUpdate = millis();
    };
}

#endif // RUNMODSTATE_H