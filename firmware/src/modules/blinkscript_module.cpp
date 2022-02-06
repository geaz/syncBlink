#include "blinkscript_module.hpp"
#include "core/audio/analyzer_constants.hpp"
#include "core/message/commands/set_display.hpp"

namespace SyncBlink
{
    BlinkScriptModule::BlinkScriptModule(LED& led, MessageBus& messageBus) : BlinkScriptModule(led, messageBus, Script())
    { }

    BlinkScriptModule::BlinkScriptModule(LED& led, MessageBus& messageBus, Script initalScript) : _led(led), _messageBus(messageBus), _currentScript(initalScript)
    {   
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(this);
        
        _runScriptView = std::make_shared<RunScriptView>();
        _invalidScriptView = std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66);
        _failSafeView = std::make_shared<IconTextView>("Fail Safe!", u8g2_font_open_iconic_thing_2x_t, 78);
    }
    
    BlinkScriptModule::~BlinkScriptModule()
    {
        _messageBus.removeMsgHandler(_scriptHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
    }

    void BlinkScriptModule::loop()
    {   
        if ((_activeScriptChanged || _blinkScript == nullptr) && _currentScript.Exists)
        {
            Commands::SetDisplay command = { _runScriptView, _currentScript.Name };
            _messageBus.trigger(command);

            _blinkScript = std::make_shared<BlinkScript>(_led, _currentScript.Content, MaxFrequency);
            _blinkScript->updateLedInfo(0, 0, _led.getLedCount());
            _blinkScript->init();
        }
    }

    void BlinkScriptModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        if (_blinkScript == nullptr) return;

        uint32_t delta = millis() - _lastLedUpdate;
        _lastLedUpdate = millis();

        setView(msg, delta);
        _blinkScript->updateAnalyzerResult(msg.volume, msg.frequency);
        _blinkScript->run(delta);
    }

    void BlinkScriptModule::onMsg(const Messages::ScriptChange& msg)
    {
        _activeScriptChanged = true;
        _currentScript = msg.script;
    }

    void BlinkScriptModule::onMsg(const Messages::MeshUpdate& msg)
    {
        _activeScriptChanged = true;
        _currentScript = msg.script;

        Serial.println("test");
        Serial.println(msg.script.Name.c_str());
        Serial.println(msg.script.Content.c_str());
    }

    bool BlinkScriptModule::checkBlinkScript()
    {
        bool valid = true;
        if (_blinkScript->isFaulted())
        {
            Commands::SetDisplay command = { _invalidScriptView, _currentScript.Name };
            _messageBus.trigger(command);
            valid = false;
        }
        return valid;
    }

    void BlinkScriptModule::setView(Messages::AnalyzerUpdate msg, uint32_t delta)
    {
        _runScriptView->delta = delta;
        _runScriptView->volume = msg.volume;
        _runScriptView->decibel = msg.decibel;
        if (msg.volume > 0 && msg.frequency > 0)
        {
            _runScriptView->dominantFrequency = msg.frequency;
            _runScriptView->setFreqBars(msg.freqBins);
        }
        else
        {
            _runScriptView->dominantFrequency = 0;
            _runScriptView->fadeFrequencyRange();
        }
    }
}