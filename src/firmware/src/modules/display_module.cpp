#include "display_module.hpp"

#include "core/views/splash_view.cpp"

#include <ESP8266WiFi.h>

namespace SyncBlink
{
    DisplayModule::DisplayModule(MessageBus& messageBus) : _messageBus(messageBus)
    {
        _scriptChangeHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(MessageType::ScriptChange, this);
        _scriptErrorHandleId = _messageBus.addMsgHandler<Messages::ScriptError>(MessageType::ScriptError, this);
        _analyzerUpdateHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(MessageType::AnalyzerUpdate, this);

        _runScriptView = std::make_shared<RunScriptView>();
        _invalidScriptView = std::make_shared<IconTextView>("Invalid script!", u8g2_font_open_iconic_check_2x_t, 66);
    }

    DisplayModule::~DisplayModule()
    {
        _messageBus.removeMsgHandler(_scriptChangeHandleId);
        _messageBus.removeMsgHandler(_scriptErrorHandleId);
        _messageBus.removeMsgHandler(_analyzerUpdateHandleId);
    }

    void DisplayModule::setup()
    {
        _display.init();
        _display.setView(std::make_shared<SplashView>());
        _display.loop();
    }

    void DisplayModule::loop()
    {
        _display.setRightStatus(WiFi.localIP().toString().c_str());
        _display.loop();
    }

    void DisplayModule::onMsg(const Messages::ScriptChange& msg)
    {
        _display.setView(_runScriptView);
        _display.setLeftStatus(msg.scriptName);
    }

    void DisplayModule::onMsg(const Messages::ScriptError& msg)
    {
        _display.setView(_invalidScriptView);
        _display.setLeftStatus(msg.scriptName);
    }

    void DisplayModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        uint32_t delta = millis() - _lastUpdate;
        _lastUpdate = millis();

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