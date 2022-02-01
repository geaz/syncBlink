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
        result.volume = SyncBlink::mapf(result.decibel < MinDB ? MinDB : result.decibel, MinDB, 0, 0, 100);
    }

    void FrequencyAnalyzer::calculateDominantFrequency(FFTData& data, AudioAnalyzerResult& result)
    {
        auto fft = ArduinoFFT<float>(data.fftReal, data.fftImg, FFTDataSize, SampleRate);
        fft.windowing(FFTWindow::Hamming, FFTDirection::Forward);
        fft.compute(FFTDirection::Forward);
        fft.complexToMagnitude();

        uint16_t highestBin = 0;
        float highestAmplitude = 0;
        for (uint16_t i = 0; i < MaxFreqBinIndex; i++)
        {
            if (data.fftReal[i] > highestAmplitude)
            {
                highestAmplitude = data.fftReal[i];
                highestBin = i;
            }
        }
        std::copy(std::begin(data.fftReal), std::begin(data.fftReal) + HalfFFTDataSize, std::begin(result.amplitudes));

        result.dominantFrequency = highestBin * ((float)SampleRate / (float)FFTDataSize);
        // Exponential Smoothing for the frequencies
        // To flatten frequency peaks
        result.dominantFrequency = EfAlpha * result.dominantFrequency + (1.0f - EfAlpha) * _lastDominantFrequency;
        _lastDominantFrequency = result.dominantFrequency;
    }
}