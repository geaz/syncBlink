#include <audio_analyzer_result.hpp>
#include "frequency_analyzer.hpp"

#include <cmath>

namespace SyncBlink
{
    namespace Api
    {
        FrequencyAnalyzer::FrequencyAnalyzer(uint64_t analyzerId, MessageBus& messageBus)
            : _analyzerId(analyzerId), _messageBus(messageBus)
        { }

        bool FrequencyAnalyzer::start()
        {            
            bool started = false;
            if(_adc.getDeviceCount() < 1) return started;

            try
            {
                if(!_adc.isStreamOpen())
                {                    
                    unsigned int bufferFrames = SyncBlink::FFTDataSize;
                    uint32_t deviceId = _adc.getDefaultOutputDevice();
                    RtAudio::DeviceInfo deviceInfo = _adc.getDeviceInfo(deviceId);

                    _parameters.deviceId = _adc.getDefaultOutputDevice();
                    _parameters.nChannels = deviceInfo.outputChannels;
                    _parameters.firstChannel = 0;
                    
                    _adc.openStream(nullptr, &_parameters, RTAUDIO_FLOAT32, SyncBlink::SampleRate, &bufferFrames, 
                        &FrequencyAnalyzer::getAnalyzerResult, (void *)this);
                }
                _adc.startStream();
                started = true;
            }
            catch (RtAudioError& e)
            {
                e.printMessage();
            }
            return started;
        }

        void FrequencyAnalyzer::stop()
        {
            if(_adc.isStreamRunning()) _adc.stopStream();
        }

        std::array<float, MaxFreqBinIndex> FrequencyAnalyzer::fftToSmoothedNormalizedFftDb(const kiss_fft_cpx* cx, uint16_t& dominantFrequency)
        {
            float maxFftVal = 0.0f;
            float minFftVal = 0.0f;
            bool firstRunDone = false;

            uint16_t maxBinIndex = 0;
            std::array<float, MaxFreqBinIndex> smoothedFft = {0};
            for(uint16_t i = 0; i < MaxFreqBinIndex; i++)
            {
                auto fftCurr = sqrtf(powf(cx[i].r, 2) + powf(cx[i].i, 2));
                if (fftCurr != 0)
                    smoothedFft[i] = EfAlpha * _prevSmoothedFft[i] + ((1.0f - EfAlpha) + fftCurr);
                else
                    smoothedFft[i] = 0;

                if(!firstRunDone || smoothedFft[i] < minFftVal) 
                {
                    firstRunDone = true;
                    minFftVal = smoothedFft[i];
                }

                if(smoothedFft[i] > maxFftVal)
                {
                    maxFftVal = smoothedFft[i];
                    maxBinIndex = i;
                }
            }

            // Normalize the values in a range of 0 and 1
            float range = maxFftVal - minFftVal;
            float scaleFactor = range + 0.0000000001f; // avoid zero division
            for(uint16_t i = 0; i < MaxFreqBinIndex; i++)
            {
                smoothedFft[i] = (smoothedFft[i] - minFftVal) / scaleFactor;
            }

            // Get the Frequency represented by the highest Bin
            dominantFrequency = (uint16_t)(maxBinIndex * ((float)SampleRate / (float)FFTDataSize));

            return smoothedFft;
        }

        /* static */ int FrequencyAnalyzer::getAnalyzerResult(void *outputBuffer, void *inputBuffer, 
            unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
        {
            FrequencyAnalyzer* freqAnalyzer = (FrequencyAnalyzer*)userData;

            auto now = std::chrono::system_clock::now();
            auto elapsed = now - freqAnalyzer->_lastUpdate;
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

            // We want to avoid flodding the mesh and network
            // Just do updates every 40ms
            if(milliseconds >= 40)
            {
                kiss_fft_cpx cx_in[FFTDataSize], cx_out[FFTDataSize];       
                kiss_fft_cfg cfg = kiss_fft_alloc(FFTDataSize, false, nullptr, nullptr);

                float signalRMS = 0;
                uint8_t channelCount = freqAnalyzer->_parameters.nChannels;
                for(uint32_t i = 0; i < FFTDataSize; i++)
                {
                    // Sum the samples of each channel
                    float sum = 0;
                    for(uint8_t j = 0; j < channelCount; j++)
                    {
                        sum += ((float *)inputBuffer)[(i * channelCount) + j];
                    }

                    float sample = sum / channelCount;
                    signalRMS += sample * sample;
                    cx_in[i].r = sample;
                    cx_in[i].i = 0;
                    cx_out[i].r = 0;
                    cx_out[i].i = 0;
                }
                
                AudioAnalyzerResult result;

                kiss_fft(cfg, cx_in, cx_out);
                kiss_fft_free(cfg);

                result.normalizedFft = freqAnalyzer->fftToSmoothedNormalizedFftDb(&cx_out[0], result.dominantFrequency);
                freqAnalyzer->_prevSmoothedFft = result.normalizedFft;

                result.decibel = 20.0f * log10(sqrtf(signalRMS / FFTDataSize));
                result.volume = (uint8_t) SyncBlink::map(result.decibel < MinDB ? MinDB : result.decibel, (float)MinDB, 0.0f, 0.0f, 100.0f);

                freqAnalyzer->_messageBus.trigger(result.ToMessage(freqAnalyzer->_analyzerId));     
                freqAnalyzer->_lastUpdate = std::chrono::system_clock::now();
            }   
            return 0;
        }
    }
}