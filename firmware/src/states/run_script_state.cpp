#ifndef RUNSCRIPTSTATE_H
#define RUNSCRIPTSTATE_H

#include "audio/analyzer_constants.hpp"
#include "event/events/analyzer_update_event.hpp"
#include "event/events/script_change_event.hpp"
#include "state.hpp"
#include "state_context.hpp"
#include "states/invalid_script_state.cpp"
#include "views/run_script_view.cpp"

#include <blink_script.hpp>

namespace SyncBlink
{
    class RunScriptState : public State,
                           public EventHandler<Events::AnalyzerUpdateEvent>,
                           public EventHandler<Events::ScriptChangeEvent>
    {
    public:
        RunScriptState(StateContext& context) : _context(context)
        {
            _runScriptView = std::make_shared<RunScriptView>();
            _scriptEventHandleId = _context.getEventBus().addEventHandler<Events::ScriptChangeEvent>(this);
            _analyzerEventHandleId = _context.getEventBus().addEventHandler<Events::AnalyzerUpdateEvent>(this);
        }

        ~RunScriptState()
        {
            _context.getEventBus().removeEventHandler(_scriptEventHandleId);
            _context.getEventBus().removeEventHandler(_analyzerEventHandleId);
        }

        void loop()
        {
            _context.getDisplay().setView(_runScriptView);
            _context.getDisplay().setLeftStatus(_scriptName);

            if (_activeScriptChanged) _context.resetState();
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
        }

        void onEvent(const Events::AnalyzerUpdateEvent& event)
        {
            if (_blinkScript == nullptr) return;

            uint32_t delta = millis() - _lastLedUpdate;
            _lastLedUpdate = millis();

            setView(event, delta);
            _blinkScript->updateAnalyzerResult(event.volume, event.frequency);
            _blinkScript->run(delta);
        }

        void onEvent(const Events::ScriptChangeEvent& event)
        {
            _activeScriptChanged = true;
        }

    private:
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
        bool _activeScriptChanged = false;

        uint32_t _scriptEventHandleId = 0;
        uint32_t _analyzerEventHandleId = 0;

        uint64_t _lastLedUpdate = millis();
    };
}

#endif // RUNSCRIPTSTATE_H