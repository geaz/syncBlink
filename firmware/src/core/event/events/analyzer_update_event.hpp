#ifndef ANALYZERUPDATEEVENT_H
#define ANALYZERUPDATEEVENT_H

#include <array>
#include <cinttypes>

namespace SyncBlink
{
    namespace Events
    {
        struct AnalyzerUpdateEvent
        {
            uint64_t analyzerId;
            float decibel;
            uint8_t volume;                   // The current registered volume
            uint16_t frequency;               // The current registered dominant frequency
            std::array<uint8_t, 32> freqBins; // The frequency amplitudes (0 = 70hz - 31 = 4186hz)
        };
    }
}

#endif // ANALYZERUPDATEEVENT_H