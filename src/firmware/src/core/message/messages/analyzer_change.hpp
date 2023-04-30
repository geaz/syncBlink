#ifndef ANALYZERCHANGEMSG_H
#define ANALYZERCHANGEMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        class AnalyzerChange : public Message
        {
        public:
            AnalyzerChange()
            {
            }
            
            AnalyzerChange(uint64_t analyzerId) : analyzerId{analyzerId}
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&analyzerId, sizeof(analyzerId));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                loadBytes(&package.body[offset], (void*)&analyzerId, sizeof(analyzerId));
            }

            MessageType getMessageType() const override
            {
                return MessageType::AnalyzerChange;
            }

            uint64_t analyzerId;
        };
    }
}

#endif // ANALYZERCHANGEMSG_H