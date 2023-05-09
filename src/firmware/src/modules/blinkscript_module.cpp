#include "blinkscript_module.hpp"

#include "core/audio/analyzer_constants.hpp"
#include "core/message/messages/script_error.hpp"

namespace SyncBlink
{
    BlinkScriptModule::BlinkScriptModule(LED& led, MessageBus& messageBus, ScriptModule& scriptModule, std::string nodeName, std::string nodeType)
        : _led(led), _messageBus(messageBus), _scriptModule(scriptModule), _nodeName(nodeName), _nodeType(nodeType), _meshLedCount(_led.getLedCount())
    {
        _meshHandleId = _messageBus.addMsgHandler<Messages::MeshUpdate>(MessageType::MeshUpdate, this);
        _scriptHandleId = _messageBus.addMsgHandler<Messages::ScriptChange>(MessageType::ScriptChange, this);
        _analyzerHandleId = _messageBus.addMsgHandler<Messages::AnalyzerUpdate>(MessageType::AnalyzerUpdate, this);
    }

    BlinkScriptModule::~BlinkScriptModule()
    {
        _messageBus.removeMsgHandler(_meshHandleId);
        _messageBus.removeMsgHandler(_scriptHandleId);
        _messageBus.removeMsgHandler(_analyzerHandleId);
    }

    void BlinkScriptModule::loop()
    {
        checkFailSafe();
        if (_inFailSafe) return;

        if ((_activeScriptChanged || _blinkScript == nullptr) && _currentScript.IsCompiled)
        {
            if (_blinkScript != nullptr) delete _blinkScript;
            _blinkScript = new BlinkScript(_led, _currentScript.BytecodePath, MaxFrequency, _nodeName, _nodeType);
            _blinkScript->updateLedInfo(_previousNodeCount, _previousLedCount, _meshLedCount);
            _blinkScript->init();

            _activeScriptChanged = false;
        }
    }

    void BlinkScriptModule::onMsg(const Messages::AnalyzerUpdate& msg)
    {
        if (_blinkScript == nullptr || _inFailSafe || checkBlinkScript()) return;

        uint32_t delta = millis() - _lastLedUpdate;
        _lastLedUpdate = millis();

        _blinkScript->updateAnalyzerResult(msg.volume, msg.frequency, msg.freqBins);
        _blinkScript->run(delta);
    }

    void BlinkScriptModule::onMsg(const Messages::ScriptChange& msg)
    {
        _inError = false;
        _inFailSafe = false;
        _activeScriptChanged = true;
        _currentScript = _scriptModule.get(msg.scriptName);
    }

    void BlinkScriptModule::onMsg(const Messages::MeshUpdate& msg)
    {
        _previousNodeCount = msg.routeNodeCount;
        _previousLedCount = msg.routeLedCount;
        _meshLedCount = msg.meshLedCount;
    }

    void BlinkScriptModule::checkFailSafe()
    {
        if(_failSafeChecked) return;

        auto rstPtr = ESP.getResetInfoPtr();
        _inFailSafe = rstPtr->reason >= 1 && rstPtr->reason <= 4 && !_failSafeChecked;
        _failSafeChecked = true;

        if (_inFailSafe)
        {
            _messageBus.trigger(Messages::ScriptError{_currentScript.Name, "TODO"});
        }
    }

    bool BlinkScriptModule::checkBlinkScript()
    {
        if (!_inError && !_activeScriptChanged && _blinkScript != nullptr && _blinkScript->isFaulted())
        {
            _messageBus.trigger(Messages::ScriptError{_currentScript.Name, "TODO"});
            _inError = true;
        }
        return _inError;
    }

    bool BlinkScriptModule::getLightMode() const
    {
        return _lightMode;
    }

    uint64_t BlinkScriptModule::getActiveAnalyzer() const
    {
        return _activeAnalyzer;
    }
}