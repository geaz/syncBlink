#ifndef MESSAGE_H
#define MESSAGE_H

#include "message_types.hpp"
#include "message_package.hpp"

#include <tuple>
#include <vector>
#include <cstring>

namespace SyncBlink
{
    struct Message
    {
        std::vector<uint8_t> toPackage() const
        {
            auto packageBody = getPackageBody();
            uint32_t bodySize = std::get<0>(packageBody);
            uint32_t packageSize = bodySize + PacketHeaderSize;

            std::vector<uint8_t> package;
            package.resize(packageSize);

            package[0] = PacketMagicBytes[0];
            package[1] = PacketMagicBytes[1];
            package[2] = PacketMagicBytes[2];
            package[3] = (uint8_t)(bodySize >> 24);
            package[4] = (uint8_t)(bodySize >> 16);
            package[5] = (uint8_t)(bodySize >> 8);
            package[6] = (uint8_t)(bodySize >> 0);
            package[7] = getMessageType();
            if (bodySize > 0)
            {
                memcpy(&package[8], std::get<1>(packageBody), bodySize);
            }

            return package;
        }

        virtual std::tuple<uint32_t, const void*> getPackageBody() const
        {
            return std::make_tuple(sizeof(*this), this);
        }

        virtual void loadPackage(MessagePackage package)
        {
            memcpy(this, &package.body[0], package.body.size());
        }

        virtual MessageType getMessageType() const { return MessageType::Base; };
    };
}

#endif // MESSAGE_H