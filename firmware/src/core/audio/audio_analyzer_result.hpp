#ifndef AUDIOANALYZERRESULT_H
#define AUDIOANALYZERRESULT_H

#include "map_funcs.hpp"
#include "analyzer_constants.hpp"
#include "core/network/get_id.hpp"
#include "core/event/events/analyzer_update_event.hpp"

#include <array>

namespace SyncBlink
{
    struct AudioAnalyzerResult
    {
        float decibel;
        uint8_t volume;
        uint16_t dominantFrequency;
        std::array<float, HalfFFTDataSize> amplitudes;

        Events::AnalyzerUpdateEvent ToEvent()
        {
            Events::AnalyzerUpdateEvent analyzerEvent = {getId(), decibel, volume, dominantFrequency};

            // Map the values and amount of amplitudes
            // to the AnalyzerUpdateEvent freqBin array (32 values in range of 0 - 255)
            // This way we only need ~ half the amount of values and instead of 4bytes per value (float)
            // just 1 byte (uint8_t). Smaller size = faster transmission
            std::array<float, MaxFreqBinIndex> copyAmplitudes;
            std::copy(std::begin(amplitudes), std::begin(amplitudes) + MaxFreqBinIndex, std::begin(copyAmplitudes));
            analyzerEvent.freqBins = SyncBlink::mapa<uint8_t, 32, float, MaxFreqBinIndex>(copyAmplitudes, 255);

            return analyzerEvent;
        }
    };
}

#endif // AUDIOANALYZERRESULT_H