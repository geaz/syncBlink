#include "frequency_analyzer.hpp"

#include "analyzer_constants.hpp"
#include "map_funcs.hpp"

#include <arduinoFFT.h>

namespace SyncBlink
{
    /**
     * @brief   On each call this method collects samples and analyzes the volume and frequency.
     *          The results are returned in the 'AudioAnalyzerResult' struct.
     */
    AudioAnalyzerResult FrequencyAnalyzer::loop()
    {
        AudioAnalyzerResult result;
        FFTData data = collectSamples();
        calculateVolumeAndBandPass(data, result);
        if (result.volume > 0) calculateDominantFrequency(data, result);
        else
            result.dominantFrequency = 0;

        return result;
    }

    FFTData FrequencyAnalyzer::collectSamples()
    {
        FFTData data;
        unsigned long startMicros = micros();
        for (int i = 0; i < FFTDataSize; i++)
        {
            data.fftReal[i] = (analogRead(A0) - 512.0f) / 512.0f;
            data.fftImg[i] = 0;
            while (micros() - startMicros < SamplePerioduSec)
            {
                // empty loop to wait
            }
            startMicros += SamplePerioduSec;
        }
        return data;
    }

    void FrequencyAnalyzer::calculateVolumeAndBandPass(FFTData& data, AudioAnalyzerResult& result)
    {
        float signalRMS = 0;
        float sample = 0;
        for (int i = 0; i < FFTDataSize; i++)
        {
            // Using Bandpass Filter here, because SyncBlink
            // samples include noise in lower frequencies.
            // Filtering them away with help of the Band-Pass-Filter
            // SyncBlink will not be able to recognize <= 100hz frequencies very well...
            sample = _bandPassFilter.filter(data.fftReal[i]);
            data.fftReal[i] = sample;
            signalRMS += sample * sample;
        }

        float signalRMSflt = sqrt(signalRMS / FFTDataSize);
        result.decibel = 20.0 * log10(signalRMSflt);
        result.volume = (uint8_t) SyncBlink::map(result.decibel < MinDB ? MinDB : result.decibel, (float)MinDB, 0.0f, 0.0f, 100.0f);
    }

    void FrequencyAnalyzer::calculateDominantFrequency(FFTData& data, AudioAnalyzerResult& result)
    {
        auto fft = ArduinoFFT<float>(data.fftReal, data.fftImg, FFTDataSize, SampleRate);
        fft.windowing(FFTWindow::Hamming, FFTDirection::Forward);
        fft.compute(FFTDirection::Forward);
        fft.complexToMagnitude();

        float maxFftVal = 0.0f;
        float minFftVal = 0.0f;
        bool firstRunDone = false;
        uint8_t maxBinIndex = 0;

        for (uint8_t i = 0; i < MaxFreqBinIndex; i++)
        {
            auto fftCurr = sqrtf(powf(data.fftReal[i], 2.0f) + powf(data.fftImg[i], 2.0f));
            if (fftCurr != 0)
                fftSmoothed[i] = EfAlpha * fftSmoothed[i] + ((1.0f - EfAlpha) + fftCurr);
            else
                fftSmoothed[i] = 0;

            if(!firstRunDone || fftSmoothed[i] < minFftVal) 
            {
                firstRunDone = true;
                minFftVal = fftSmoothed[i];
            }

            if(fftSmoothed[i] > maxFftVal)
            {
                maxFftVal = fftSmoothed[i];
                maxBinIndex = i;
            }
        }

        // Normalize the values in a range of 0 and 1
        float range = maxFftVal - minFftVal;
        float scaleFactor = range + 0.0000000001f; // avoid zero division
        for(uint16_t i = 0; i < MaxFreqBinIndex; i++)
        {
            fftSmoothed[i] = (fftSmoothed[i] - minFftVal) / scaleFactor;
            result.normalizedFft[i] = fftSmoothed[i];
        }

        // Get the Frequency represented by the maxBinIndex
        result.dominantFrequency = maxBinIndex * ((float)SampleRate / (float)FFTDataSize);
    }
}