#include "script_runner.hpp"

namespace SyncBlink
{
    ScriptRunner::ScriptRunner(EventBus& eventBus, LED& led) : _eventBus(eventBus), _led(led)
    {
        _scriptHandleId = _eventBus.addEventHandler<Events::ScriptChangeEvent>(this, true);
        _analyzerHandleId = _eventBus.addEventHandler<Events::AnalyzerUpdateEvent>(this, true);
    }

    ScriptRunner::~ScriptRunner()
    {
        _eventBus.removeEventHandler(_scriptHandleId);
        _eventBus.removeEventHandler(_analyzerHandleId);
    }

    void ScriptRunner::onEvent(const Events::ScriptChangeEvent& event)
    {
        Serial.println("drin");
        _blinkScript = std::make_shared<BlinkScript>(_led, event.script.Content);
    }

    void ScriptRunner::onEvent(const Events::AnalyzerUpdateEvent& event)
    {
        
    }
}