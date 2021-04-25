#ifndef FREQUENCYANALYZER_H
#define FREQUENCYANALYZER_H

#include "audio_analyzer_result.hpp"
#include "band_pass_filter.hpp"
#include "frequency_infos.hpp"

#include <math.h>

namespace SyncBlink
{
    static const float EfAlpha = 0.4f;
    static const int8_t MinDB = -35;

    struct FFTData
    {
        float fftReal[FFTDataSize];
        float fftImg[FFTDataSize];
    };

    class FrequencyAnalyzer
    {
    public:
        AudioAnalyzerResult loop();

    private:
        FFTData collectSamples();
        void calculateVolumeAndBandPass(FFTData& data, AudioAnalyzerResult& result);
        void calculateDominantFrequency(FFTData& data, AudioAnalyzerResult& result);

        float _lastDominantFrequency = 0;
        BandPassFilter _bandPassFilter;
    };
}

#endif // FREQUENCYANALYZER_H