#ifndef CFREQUENCYANALYZER_H
#define CFREQUENCYANALYZER_H

#include <message_bus.hpp>
#include <chrono>
#include <array>
#include <functional>
#include <RtAudio.h>
#include <kiss_fft.h>
#include <message_bus.hpp>
#include <analyzer_constants.hpp>
#include <band_pass_filter.hpp>

namespace SyncBlink
{
    namespace Api
    {
        static const float EfAlpha = 0.7f;
        static const int8_t MinDB = -50;

        class FrequencyAnalyzer
        {
            public:
                FrequencyAnalyzer(uint64_t analyzerId, MessageBus& messageBus);

                bool start();
                void stop();

            private:
                std::array<float, MaxFreqBinIndex> fftToSmoothedNormalizedFftDb(const kiss_fft_cpx* cx, uint16_t& dominantFrequency);                
                static int getAnalyzerResult(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, 
                    double streamTime, RtAudioStreamStatus status, void *userData);

                uint64_t _analyzerId;
                MessageBus& _messageBus;

                RtAudio _adc = RtAudio(RtAudio::Api::WINDOWS_WASAPI);
                RtAudio::StreamParameters _parameters;

                BandPassFilter _bandPassFilter;
                std::chrono::system_clock::time_point _lastUpdate;
                std::array<float, MaxFreqBinIndex> _prevSmoothedFft = {0};
        };
    }
}

#endif // CFREQUENCYANALYZER_H