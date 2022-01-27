#ifndef RUNSCRIPTSTATE_H
#define RUNSCRIPTSTATE_H

#include "audio/audio_analyzer_result.hpp"
#include "audio/frequency_analyzer.hpp"
#include "event/events/script_change_event.hpp"
#include "state.hpp"
#include "state_context.hpp"
#include "states/invalid_script_state.cpp"
#include "views/run_script_view.cpp"

#include <blink_script.hpp>

namespace SyncBlink
{
    class RunScriptState : public State, public EventHandler<Events::ScriptChangeEvent>
    {
    public:
        RunScriptState(StateContext& context) : _context(context)
        {
            _runScriptView = std::make_shared<RunScriptView>();
            _scriptEventHandleId = _context.getEventBus().addEventHandler<Events::ScriptChangeEvent>(this);
        }

        ~RunScriptState()
        {
            _context.getEventBus().removeEventHandler(_scriptEventHandleId);
        }

        void loop()
        {
            _context.getDisplay().setView(_runScriptView);
            _context.getDisplay().setLeftStatus(_scriptName);

            if (_activeScriptChanged)
                _context.resetState();
            else if (_blinkScript == nullptr)
            {
                auto script = _context.getScriptManager().getActiveScript();
                if (script.Exists)
                {
                    _scriptName = script.Name;
                    _blinkScript = std::make_shared<BlinkScript>(_context.getLed(), script.Content, MaxFrequency);
                    _blinkScript->updateLedInfo(0, 0, 9);
                    _blinkScript->init();
                }
            }
            else
                handleMicrophoneSource();
        }

        void onEvent(const Events::ScriptChangeEvent& event)
        {
            _activeScriptChanged = true;
        }

    private:
        void handleMicrophoneSource()
        {
            if (checkBlinkScript())
            {
                AudioAnalyzerResult result = _frequencyAnalyzer.loop();
                Events::AnalyzerUpdateEvent event = result.ToEvent();

                uint32_t delta = millis() - _lastLedUpdate;
                _lastLedUpdate = millis();

                setView(event, delta);
                _blinkScript->updateAnalyzerResult(result.volume, result.dominantFrequency);
                _blinkScript->run(delta);
            }
        }

        bool checkBlinkScript()
        {
            bool valid = true;
            if (_blinkScript->isFaulted())
            {
                _context.changeState(std::make_shared<InvalidScriptState>(_context));
                valid = false;
            }
            return valid;
        }

        void setView(Events::AnalyzerUpdateEvent event, uint32_t delta)
        {
            _runScriptView->delta = delta;
            _runScriptView->volume = event.volume;
            _runScriptView->decibel = event.decibel;
            if (event.volume > 0 && event.frequency > 0)
            {
                _runScriptView->dominantFrequency = event.frequency;
                _runScriptView->setFreqBars(event.freqBins);
            }
            else
            {
                _runScriptView->dominantFrequency = 0;
                _runScriptView->fadeFrequencyRange();
            }
        }

        StateContext& _context;
        std::shared_ptr<BlinkScript> _blinkScript;
        std::shared_ptr<RunScriptView> _runScriptView;

        std::string _scriptName;
        uint32_t _scriptEventHandleId = 0;
        bool _activeScriptChanged = false;

        FrequencyAnalyzer _frequencyAnalyzer;
        uint64_t _lastLedUpdate = millis();
    };
}

#endif // RUNSCRIPTSTATE_H