#ifndef BLINKSCRIPT_H
#define BLINKSCRIPT_H

#include "script_built_ins.hpp"
#include "source/espfile_bytecode_source.hpp"
#include "vm/vm.hpp"

#include <deque>
#include <led.hpp>
#include <memory>
#include <tuple>
#include <vector>

namespace SyncBlink
{
    static const uint8_t TotalFreqBins = 48;

    class BlinkScript
    {
    public:
        BlinkScript(LED& led, std::string bytecodePath, uint16_t maxFreq, std::string nodeName, std::string nodeType);

        void init();
        void run(const uint8_t delta);
        void updateLedInfo(const uint16_t previousNodeCount, const uint32_t previousLedCount, const uint32_t meshLedCount);
        void updateAnalyzerResult(const uint8_t volume, const uint16_t dominantFrequency, const std::array<uint8_t, TotalFreqBins>& freqBin);

        bool isFaulted();
        LED& getLed();
        void setDelay(uint32_t delay);

    private:
        bool checkEvalError(const std::string& step, bool hasError, std::tuple<LINETYPE, std::string> error);
        void saveAddToScope(const std::string& identifier, Value value);
        void saveAddToScope(const std::string& identifier, std::string stringValue);
        void saveAddFreqBin();

        LED& _led;
        VM _vm;
        std::shared_ptr<Program> _program;
        std::shared_ptr<EspFileBytecodeSource> _source;

        bool _faulted = false;
        std::tuple<LINETYPE, std::string> _error = std::make_tuple(0, "");

        uint8_t _lastVolume = 0;
        uint16_t _lastFrequency = 0;
        std::unique_ptr<ArrayObj> _freqBin;

        uint32_t _delay = 0;
        std::deque<std::tuple<uint8_t, uint16_t>> _resultDeque;
    };
}

#endif // BLINKSCRIPT_H