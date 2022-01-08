#include "blink_script.hpp"

#include "parser/parser.hpp"
#include "scanner/scanner.hpp"
#include "vm/compiler.hpp"
#include "vm/model/objects/native_function_object.hpp"
#include "vm/model/value.hpp"
#include "audio/analyzer_constants.hpp"

namespace SyncBlink
{
    BlinkScript::BlinkScript(LED& led, const std::string& script, uint16_t maxFreq) : _led(led)
    {
        auto parser = Parser(script);
        auto programAst = parser.parse();

        if (parser.hasError())
        {
            _faulted = true;
            auto error = parser.getError();
            Serial.println("Line: " + String(std::get<0>(error)) + ": " + std::get<1>(error).c_str());
            return;
        }

        auto compiler = Compiler();
        _program = compiler.compile(programAst);

        if (compiler.hasError())
        {
            _faulted = true;
            auto error = compiler.getError();
            Serial.println("Line: " + String(std::get<0>(error)) + ": " + std::get<1>(error).c_str());
            return;
        }

        _vm.run(_program);
        if (checkEvalError("preInit", _vm.hasError(), _vm.getError()))
            return;

        // Add global funcs
        BuiltIns::println(_vm);
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
    }

    void BlinkScript::init()
    {
        if (isFaulted())
            return;
        _vm.executeFun("init", {});
        checkEvalError("init", _vm.hasError(), _vm.getError());
    }

    void BlinkScript::run(const uint8_t delta)
    {
        if (isFaulted())
            return;
        _vm.executeFun("update", {Value((float)delta)});
        checkEvalError("update", _vm.hasError(), _vm.getError());
    }

    void BlinkScript::updateLedInfo(const uint16_t previousNodeCount, const uint32_t previousLedCount,
                                    const uint32_t meshLedCount)
    {
        if (isFaulted())
            return;
        _vm.getFrame().addSet("pNodeC", Value((float)previousNodeCount));
        _vm.getFrame().addSet("pLedC", Value((float)previousLedCount));
        _vm.getFrame().addSet("mLedC", Value((float)meshLedCount));
    }

    void BlinkScript::updateAnalyzerResult(const uint8_t volume, const uint16_t dominantFrequency)
    {
        if (isFaulted())
            return;

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
            _vm.getFrame().addSet("lVol", Value((float)_lastVolume));
            _vm.getFrame().addSet("lFreq", Value((float)_lastFrequency));
            _vm.getFrame().addSet("vol", Value((float)std::get<0>(result)));
            _vm.getFrame().addSet("freq", Value((float)std::get<1>(result)));

            _lastVolume = std::get<0>(result);
            _lastFrequency = std::get<1>(result);
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
        if (_delay == 0)
            _delay = delay;
    }

    std::string BlinkScript::getScriptName()
    {
        std::string scriptName = "(undefined)";
        auto scriptValue = _vm.getFrame().get("scriptName");
        if (scriptValue.getType() != ValueType::NIL)
            scriptName = static_cast<StringObj*>(scriptValue.object)->getString();
        return scriptName;
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
        if (isFaulted())
            return;
        if (!_vm.getFrame().addSet(identifier, value))
        {
            _faulted = true;
            _error = std::make_tuple<int, std::string>(-1, "Variable '" + identifier + "' already defined!");
        }
    }
}