#ifndef RUNMODSTATE_H
#define RUNMODSTATE_H

#include <mappings.hpp>
#include <blinkscript/blink_script.hpp>
#include <audio/frequency_analyzer.hpp>
#include <network/websocket/messages/audio_analyzer_message.hpp>
#include <network/websocket/messages/server_messages.hpp>

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
                _socketEventHandleId = context.getSocketServer()
                    .messageEvents
                    .addEventHandler([this](Client::Message command) 
                    { 
                        handleExternalSource(command); 
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
                _context.getSocketServer()
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
                
                if(_activeModChanged) context.resetState();
                else handleMicrophoneSource(context.getSocketServer());
            }

        private:
            void handleMicrophoneSource(SocketServer& socketServer)
            {   
                if(checkBlinkScript() && _context.getModManager().getActiveSource() == AudioAnalyzerSource::Station)
                {
                    AudioAnalyzerResult result = _frequencyAnalyzer.loop();
                    Server::Message message = { millis(), Server::ANALYZER_UPDATE };
                    message.analyzerMessage = result.ToMessage();

                    uint32_t delta = millis() - _lastLedUpdate;
                    _lastLedUpdate = millis();

                    socketServer.broadcast(message);
                    setView(message.analyzerMessage, delta);
                    _blinkScript->updateAnalyzerResult(result.volume, result.dominantFrequency);
                    _blinkScript->run(delta);
                }
            }

            void handleExternalSource(Client::Message& clientMessage)
            {
                if(checkBlinkScript()
                && _context.getModManager().getActiveSource() != AudioAnalyzerSource::Station
                && clientMessage.messageType == Client::MessageType::EXTERNAL_ANALYZER)
                {
                    Server::Message message = { millis(), Server::ANALYZER_UPDATE };
                    message.analyzerMessage = clientMessage.audioAnalyzerMessage;

                    uint32_t delta = millis() - _lastLedUpdate;
                    _lastLedUpdate = millis();

                    _context.getSocketServer().broadcast(message);
                    setView(message.analyzerMessage, delta);

                    _blinkScript->updateAnalyzerResult(message.analyzerMessage.volume, message.analyzerMessage.frequency);
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

            FrequencyAnalyzer _frequencyAnalyzer;
            uint64_t _lastLedUpdate = millis();
    };
}

#endif // RUNMODSTATE_H