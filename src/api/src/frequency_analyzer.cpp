#include <cmath>
#include <audio_analyzer_result.hpp>
#include "frequency_analyzer.hpp"

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

        std::array<float, HalfFFTDataSize> FrequencyAnalyzer::calculateAmplitudes(const kiss_fft_cpx* cx) const
        {
            std::array<float, HalfFFTDataSize> amplitudes;
            for(uint16_t i = 0; i < HalfFFTDataSize; i++)
            {
                amplitudes[i] = sqrtf(powf(cx[i].r, 2) + powf(cx[i].i, 2));
            }
            return amplitudes;
        }

        uint16_t FrequencyAnalyzer::getDominantFrequency(std::array<float, HalfFFTDataSize> amplitudes)
        {
            uint16_t highestBin = 0;
            float highestAmplitude = 0;
            for(uint16_t i = 0; i < MaxFreqBinIndex; i++)
            {
                if(amplitudes[i] > highestAmplitude)
                {
                    highestAmplitude = amplitudes[i];
                    highestBin = i;
                }
            }

            // Get the Frequency represented by the highest Bin
            float dominantFrequency = highestBin * ((float)SampleRate / (float)FFTDataSize);            
            // Exponential Smoothing for the frequencies
            // To flatten frequency peaks
            dominantFrequency = EfAlpha * dominantFrequency + (1.0f - EfAlpha) * _lastDominantFrequency;
            
            _lastDominantFrequency = dominantFrequency;
            return (uint16_t)dominantFrequency;
        }

        /* static */ int FrequencyAnalyzer::getAnalyzerResult(void *outputBuffer, void *inputBuffer, 
            unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
        {
            FrequencyAnalyzer* freqAnalyzer = (FrequencyAnalyzer*)userData;

            auto now = std::chrono::system_clock::now();
            auto elapsed = now - freqAnalyzer->_lastUpdate;
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

            // We want to avoid flodding the mesh and network
            // Just do updates every 30ms
            if(milliseconds >= 30)
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

                    // We want to filter out the bass of a few songs
                    // Otherwise the bass would be always the dominant low frequency
                    // Does not look very cool....
                    float sample = freqAnalyzer->_bandPassFilter.filter(sum / channelCount);
                    signalRMS += sample * sample;
                    cx_in[i].r = sample;
                    cx_in[i].i = 0;
                    cx_out[i].r = 0;
                    cx_out[i].i = 0;
                }
                
                kiss_fft(cfg, cx_in, cx_out);
                kiss_fft_free(cfg);

                AudioAnalyzerResult result;
                result.decibel = 20.0f * log10(sqrtf(signalRMS / FFTDataSize));
                result.volume = (uint8_t) SyncBlink::mapf(result.decibel < MinDB ? MinDB : result.decibel, MinDB, 0, 0, 100);
                result.amplitudes = freqAnalyzer->calculateAmplitudes(&cx_out[0]);
                result.dominantFrequency = freqAnalyzer->getDominantFrequency(result.amplitudes);

                freqAnalyzer->_messageBus.trigger(result.ToMessage(freqAnalyzer->_analyzerId));     
                freqAnalyzer->_lastUpdate = std::chrono::system_clock::now();
            }   
            return 0;
        }
    }
}