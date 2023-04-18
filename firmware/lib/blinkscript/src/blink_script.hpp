#ifndef BLINKSCRIPT_H
#define BLINKSCRIPT_H

#include "vm/vm.hpp"
#include "script_built_ins.hpp"

#include <deque>
#include <memory>
#include <tuple>
#include <vector>
#include <led.hpp>
 
namespace SyncBlink
{
    class BlinkScript
    {
    public:
        BlinkScript(LED& led, const std::string& script, uint16_t maxFreq);

        void init();
        void run(const uint8_t delta);
        void updateLedInfo(const uint16_t previousNodeCount, const uint32_t previousLedCount,
                           const uint32_t meshLedCount);
        void updateAnalyzerResult(const uint8_t volume, const uint16_t dominantFrequency);

        bool isFaulted();
        LED& getLed();
        void setDelay(uint32_t delay);

    private:
        bool checkEvalError(const std::string& step, bool hasError, std::tuple<int, std::string> error);
        void saveAddToScope(const std::string& identifier, Value value);

        LED& _led;
        VM _vm;
        Program _program;

        bool _faulted = false;
        std::tuple<int, std::string> _error = std::make_tuple(-99, "");

        uint8_t _lastVolume = 0;
        uint16_t _lastFrequency = 0;

        uint32_t _delay = 0;
        std::deque<std::tuple<uint8_t, uint16_t>> _resultDeque;
    };
}

#endif // BLINKSCRIPT_H