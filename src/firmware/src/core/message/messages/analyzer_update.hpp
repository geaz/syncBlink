#ifndef ANALYZERUPDATEMSG_H
#define ANALYZERUPDATEMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"
#include "core/audio/analyzer_constants.hpp"

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
                addBytes(package, (void*)&freqBins[0], TotalResultFreqBins * sizeof(uint8_t));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&analyzerId, sizeof(analyzerId));
                offset += loadBytes(&package.body[offset], (void*)&decibel, sizeof(decibel));
                offset += loadBytes(&package.body[offset], (void*)&volume, sizeof(volume));
                offset += loadBytes(&package.body[offset], (void*)&frequency, sizeof(frequency));
                loadBytes(&package.body[offset], (void*)&freqBins[0], TotalResultFreqBins * sizeof(uint8_t));
            }

            MessageType getMessageType() const override
            {
                return MessageType::AnalyzerUpdate;
            }

            uint64_t analyzerId;
            float decibel;
            uint8_t volume;
            uint16_t frequency;
            std::array<uint8_t, TotalResultFreqBins> freqBins;
        };
    }
}

#endif // ANALYZERUPDATEMSG_H