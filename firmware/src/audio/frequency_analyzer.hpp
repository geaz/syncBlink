#ifndef FREQUENCYANALYZER_H
#define FREQUENCYANALYZER_H

#include <math.h>
#include <memory>

#include "audio_analyzer_result.hpp"
#include "band_pass_filter.hpp"

namespace SyncBlink
{    
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