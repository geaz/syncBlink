#include "blink_script.hpp"

#include "program/bytecode_loader.hpp"
#include "program/model/objects/native_function_object.hpp"
#include "program/model/value.hpp"

namespace SyncBlink
{
    BlinkScript::BlinkScript(LED& led, std::string bytecodePath, uint16_t maxFreq, std::string nodeName, std::string nodeType) : _led(led)
    {
        _source = std::make_shared<EspFileBytecodeSource>(bytecodePath);
        auto byteCodeLoader = ByteCodeLoader(_source);
        _program = std::make_shared<Program>(byteCodeLoader.getProgram());
        _vm.run((const Program&)*_program.get());

        if (checkEvalError("preInit", _vm.hasError(), _vm.getError())) return;

        // Clear LED groups, if set in previous running scripts
        led.clearGroups();

        // Add global funcs
        BuiltIns::println(_vm);
        BuiltIns::setNumArray(_vm);
        BuiltIns::setGroups(*this, _vm);
        BuiltIns::setLinearGroups(*this, _vm);
        BuiltIns::clearGroups(*this, _vm);
        BuiltIns::setDelay(*this, _vm);
        BuiltIns::getLed(*this, _vm);
        BuiltIns::setLeds(*this, _vm);
        BuiltIns::setAllLeds(*this, _vm);
        BuiltIns::map(_vm);
        BuiltIns::xrgb(_vm);
        BuiltIns::xhsv(_vm);

        // Add global vars
        saveAddToScope("maxF", Value((float)maxFreq));
        saveAddToScope("nLedC", Value((float)_led.getLedCount()));
        saveAddToScope("mLedC", Value((float)_led.getLedCount()));
        saveAddToScope("pLedC", Value(0.0f));
        saveAddToScope("pNodeC", Value(0.0f));
        saveAddToScope("lVol", Value(0.0f));
        saveAddToScope("lFreq", Value(0.0f));
        saveAddToScope("vol", Value(0.0f));
        saveAddToScope("freq", Value(0.0f));
        saveAddToScope("name", nodeName);
        saveAddToScope("type", nodeType);
        saveAddFreqBin();
    }

    void BlinkScript::init()
    {
        if (isFaulted()) return;
        _vm.executeFun("init", {});
        checkEvalError("init", _vm.hasError(), _vm.getError());
    }

    void BlinkScript::run(const uint8_t delta)
    {
        if (isFaulted()) return;
        _vm.executeFun("update", {Value((float)delta)});
        checkEvalError("update", _vm.hasError(), _vm.getError());
    }

    void BlinkScript::updateLedInfo(const uint16_t previousNodeCount, const uint32_t previousLedCount, const uint32_t meshLedCount)
    {
        if (isFaulted()) return;
        _vm.setGlobal("pNodeC", Value((float)previousNodeCount));
        _vm.setGlobal("pLedC", Value((float)previousLedCount));
        _vm.setGlobal("mLedC", Value((float)meshLedCount));
    }

    void BlinkScript::updateAnalyzerResult(const uint8_t volume, const uint16_t dominantFrequency, const std::array<uint8_t, TotalFreqBins>& freqBin)
    {
        if (isFaulted()) return;

        bool updateResult = true;
        std::tuple<uint8_t, uint16_t> result = std::make_tuple(volume, dominantFrequency);

        if (_delay > 0)
        {
            _resultDeque.push_back(result);
            if (_resultDeque.size() > _delay)
            {
                result = _resultDeque.front();
                _resultDeque.pop_front();
            }
            else
                updateResult = false;
        }

        if (updateResult)
        {
            _vm.setGlobal("lVol", Value((float)_lastVolume));
            _vm.setGlobal("lFreq", Value((float)_lastFrequency));
            _vm.setGlobal("vol", Value((float)std::get<0>(result)));
            _vm.setGlobal("freq", Value((float)std::get<1>(result)));

            _lastVolume = std::get<0>(result);
            _lastFrequency = std::get<1>(result);

            bool resetBin = (float)std::get<0>(result) == 0;
            for (int i = 0; i < TotalFreqBins; i++)
            {
                if (resetBin) _freqBin->getValues()[i] = Value((float)0);
                else
                    _freqBin->getValues()[i] = Value((float)freqBin[i]);
            }
        }
    }

    bool BlinkScript::isFaulted()
    {
        return _faulted;
    }

    LED& BlinkScript::getLed()
    {
        return _led;
    }

    void BlinkScript::setDelay(uint32_t delay)
    {
        if (_delay == 0) _delay = delay;
    }

    bool BlinkScript::checkEvalError(const std::string& step, bool hasError, std::tuple<int, std::string> error)
    {
        if (hasError)
        {
            _faulted = true;
            _error = error;

            Serial.print("Error on '" + String(step.c_str()) + "'!");
            Serial.print(" Line " + String(std::get<0>(error)) + ": ");
            Serial.println(std::get<1>(error).c_str());
        }
        return _faulted;
    }

    void BlinkScript::saveAddToScope(const std::string& identifier, Value value)
    {
        if (isFaulted()) return;
        _vm.setGlobal(identifier, value);
    }

    void BlinkScript::saveAddToScope(const std::string& identifier, std::string stringValue)
    {
        auto ptr = std::unique_ptr<StringObj>(new StringObj(stringValue));
        auto value = Value(ptr.get());
        _program->addConstant(value, std::move(ptr));
        saveAddToScope(identifier, value);
    }

    void BlinkScript::saveAddFreqBin()
    {
        std::vector<Value> freqBin;
        for (int i = 0; i < TotalFreqBins; i++)
        {
            freqBin.push_back(Value((float)0));
        }
        _freqBin = std::unique_ptr<ArrayObj>(new ArrayObj(freqBin));
        saveAddToScope("freqBin", Value(_freqBin.get()));
    }
}