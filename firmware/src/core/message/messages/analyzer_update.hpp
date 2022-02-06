#ifndef ANALYZERUPDATEMSG_H
#define ANALYZERUPDATEMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <array>
#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        struct AnalyzerUpdate : public Message
        {
            AnalyzerUpdate(){}
            AnalyzerUpdate(uint64_t _analyzerId, float _decibel, uint8_t _volume, uint16_t _frequency)
                : analyzerId{_analyzerId}, decibel{_decibel}, volume{_volume}, frequency{_frequency} {}

            MessageType getMessageType() const { return MessageType::AnalyzerUpdate; }

            uint64_t analyzerId;
            float decibel;
            uint8_t volume;                   // The current registered volume
            uint16_t frequency;               // The current registered dominant frequency
            std::array<uint8_t, 32> freqBins; // The frequency amplitudes (0 = 70hz - 31 = 4186hz)
        };
    }
}

#endif // ANALYZERUPDATEMSG_H