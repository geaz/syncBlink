#ifndef RAWBYTESMSG_H
#define RAWBYTESMSG_H

#include "core/message/message.hpp"
#include "core/message/message_types.hpp"

#include <vector>
#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        class RawBytes : public Message
        {
        public:
            RawBytes()
            {
            }

            std::vector<uint8_t> getPackageBody() const override
            {
                std::vector<uint8_t> package;
                addBytes(package, (void*)&recipientId, sizeof(recipientId));
                addBytes(package, (void*)&byteCount, sizeof(byteCount));
                addBytes(package, (void*)&bytes[0], byteCount * sizeof(uint8_t));

                return package;
            }

            void loadPackage(MessagePackage package) override
            {
                uint32_t offset = 0;
                offset += loadBytes(&package.body[offset], (void*)&recipientId, sizeof(recipientId));
                offset += loadBytes(&package.body[offset], (void*)&byteCount, sizeof(byteCount));
                
                bytes.resize(byteCount);
                loadBytes(&package.body[offset], (void*)&bytes[0], byteCount * sizeof(uint8_t));
            }

            MessageType getMessageType() const override
            {
                return MessageType::RawBytes;
            }

            uint8_t byteCount;
            uint64_t recipientId = 0;
            std::vector<uint8_t> bytes;
        };
    }
}

#endif // RAWBYTESMSG_H