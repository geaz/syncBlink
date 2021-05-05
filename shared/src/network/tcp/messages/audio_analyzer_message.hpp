#ifndef AUDIOANALYZERMESSAGE_H
#define AUDIOANALYZERMESSAGE_H

#include <array>
#include <cinttypes>

namespace SyncBlink
{
    /**
     * @brief   The *AudioAnalyzerMessage* contains the frequency analyzer
     *          information of the currently selected source.
     */
    struct AudioAnalyzerMessage
    {
        float decibel;
        uint8_t volume;                   // The current registered volume
        uint16_t frequency;               // The current registered dominant frequency
        std::array<uint8_t, 32> freqBins; // The frequency amplitudes (0 = 70hz - 31 = 4186hz)
    };
}

#endif // AUDIOANALYZERMESSAGE_H