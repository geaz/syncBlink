#ifndef AUDIOANALYZERRESULT_H
#define AUDIOANALYZERRESULT_H

#include "analyzer_constants.hpp"
#include "core/message/messages/analyzer_update.hpp"
#include "map_funcs.hpp"

#include <array>
#include <algorithm>

namespace SyncBlink
{
    struct AudioAnalyzerResult
    {
        float decibel;
        uint8_t volume;
        uint16_t dominantFrequency;
        // An array of the FFT bin values converted to DB for visualization in range of 0 and 1
        std::array<float, MaxFreqBinIndex> normalizedFft{ {0} };

        Messages::AnalyzerUpdate ToMessage(uint64_t analyzerId)
        {
            Messages::AnalyzerUpdate analyzerMsg = {analyzerId, decibel, volume, dominantFrequency};

            for(uint8_t i = 0; i < MaxFreqBinIndex; i++)
            {
                bool found = false;
                float weight = 0.0f;
                for(uint8_t j = 0; j < (uint8_t)aWeightFrequency.size(); j++)
                {
                    if(aWeightFrequency[j] > (i + 1) * (MaxFrequency / MaxFreqBinIndex))
                    {
                        found = true;
                        weight = aWeight[j];
                        break;
                    }
                }
                if(!found) weight = aWeight[aWeight.size() - 1];
                normalizedFft[i] = normalizedFft[i] * weight;
            }

            analyzerMsg.freqBins = SyncBlink::getScaledMovingAverage<float, uint8_t, MaxFreqBinIndex, MaxFreqBinIndex-TotalResultFreqBins>(normalizedFft, 0, 255);
            return analyzerMsg;
        }

    private:
        const std::array<float, 46> aWeightFrequency{{
            100, 200, 300, 400,
            500, 600, 700, 800, 900, 1000,
            1100, 1200, 1300, 1400, 1500, 1600,
            1700, 1800, 1900, 2000, 2100, 2200,
            2300, 2400, 2500, 2600, 2700, 2800,
            2900, 3000, 3100, 3200, 3300, 3400,
            3500, 3600, 3700, 3800, 3900, 4000,
            4100, 4200, 4300, 4400, 4500, 4600
        }};

        const std::array<float, 46> aWeight{{
            0.05f, 0.1f, 0.15f, 0.2f,
            0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.5f,
            0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 
            1.1f, 1.2f, 1.3f, 1.4f, 1.3f, 1.2f,
            1.1f, 1.1f, 1.1f, 1.1f, 1.1f, 1.1f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f,
            0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f
        }};


        /*
        const std::array<float, 34> aWeightFrequency{{
            10, 12.5, 16, 20, 
            25, 31.5, 40, 50, 
            63, 80, 100, 125, 
            160, 200, 250, 315, 
            400, 500, 630, 800, 
            1000, 1250, 1600, 2000, 
            2500, 3150, 4000, 5000,
            6300, 8000, 10000, 12500, 
            16000, 20000 
        }};

        const std::array<float, 34> aWeightDecibels{{
            -70.4f, -63.4f, -56.7f, -50.5f, 
            -44.7f, -39.4f, -34.6f, -30.2f, 
            -26.2f, -22.5f, -19.1f, -16.1f, 
            -13.4f, -10.9f, -8.6f, -6.6f,
            -4.8f, -3.2f, -1.9f, -0.8f,
            0.0f, 0.6f, 1.0f, 1.2f,
            1.3f, 1.2f, 1.0f, 0.5f,
            -0.1f, -1.1f, -2.5f, -4.3f,
            -6.6f, -9.3f
        }};
        */
    };
}

#endif // AUDIOANALYZERRESULT_H