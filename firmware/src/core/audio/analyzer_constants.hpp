#ifndef ANALYZERCONSTANTS_H
#define ANALYZERCONSTANTS_H

#include <cinttypes>
#include <math.h>

namespace SyncBlink
{
    // We want recognize 4186hz max - setting the sample rate to 9000 will provide us with bins up to 4500hz
    // The ESP8266 is able to sample up to a rate of ~10000
    static const uint16_t SampleRate = 9000;
    static const uint64_t SamplePerioduSec = (uint64_t)round(1000000 * (1.0 / SampleRate)); // micro seconds between two samples
    static const uint16_t FFTDataSize = 128;
    static const uint16_t HalfFFTDataSize = FFTDataSize / 2;

    // https://en.wikipedia.org/wiki/Audio_frequency
    static const uint16_t MaxFrequency = 4186; // Highest note on a standard 88-key piano.
    static const uint8_t MaxFreqBinIndex = (uint8_t)((float)MaxFrequency / ((float)SampleRate / (float)FFTDataSize));

    static const float EfAlpha = 0.4f;
    static const int8_t MinDB = -30;
}

#endif // ANALYZERCONSTANTS_H