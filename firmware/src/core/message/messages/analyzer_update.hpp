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
        class AnalyzerUpdate : public Message
        {
        public:
            AnalyzerUpdate()
            {
            }
            
            AnalyzerUpdate(uint64_t _analyzerId, float _decibel, uint8_t _volume, uint16_t _frequency)
                : analyzerId{_analyzerId}, decibel{_decibel}, volume{_volume}, frequency{_frequency}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&analyzerId, sizeof(analyzerId));
                addBytes(package, (void*)&decibel, sizeof(decibel));
                addBytes(package, (void*)&volume, sizeof(volume));
                addBytes(package, (void*)&frequency, sizeof(frequency));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&analyzerId, sizeof(analyzerId));
                offset += loadBytes(&package.body[offset], (void*)&decibel, sizeof(decibel));
                offset += loadBytes(&package.body[offset], (void*)&volume, sizeof(volume));
                loadBytes(&package.body[offset], (void*)&frequency, sizeof(frequency));
            }

            MessageType getMessageType() const override
            {
                return MessageType::AnalyzerUpdate;
            }

            uint64_t analyzerId;
            float decibel;
            uint8_t volume;                   // The current registered volume
            uint16_t frequency;               // The current registered dominant frequency
            std::array<uint8_t, 32> freqBins; // The frequency amplitudes (0 = 70hz - 31 = 4186hz)
        };
    }
}

#endif // ANALYZERUPDATEMSG_H