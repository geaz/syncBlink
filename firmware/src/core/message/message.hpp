#ifndef MESSAGE_H
#define MESSAGE_H

#include "message_types.hpp"
#include "message_package.hpp"

#include <vector>
#include <cstring>

namespace SyncBlink
{
    class Message
    {
    public:
        std::vector<uint8_t> toPackage() const
        {
            auto packageBody = getPackageBody();
            uint32_t bodySize = packageBody.size();
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
                memcpy(&package[8], &packageBody[0], bodySize);
            }

            return package;
        }

        virtual std::vector<uint8_t> getPackageBody() const
        {
            std::vector<uint8_t> package;
            addBytes(package, (void*) this, sizeof(*this));
            return package;
        }

        virtual void loadPackage(MessagePackage package)
        {
            loadBytes(&package.body[0], (void*) this, sizeof(*this));
        }

        virtual MessageType getMessageType() const { return MessageType::Base; };

    protected:
        void addBytes(std::vector<uint8_t>& byteVec, const void* dataPtr, int32_t dataSize) const
        {
            uint32_t curSize = byteVec.size();
            byteVec.resize(curSize + dataSize);

            memcpy(&byteVec[curSize], dataPtr, dataSize);
        }

        uint32_t loadBytes(void* srcPtr, void* targetPtr, int32_t targetSize)
        {
            memcpy(targetPtr, srcPtr, targetSize);
            return targetSize;
        }

        void addStringBytes(std::vector<uint8_t>& byteVec, std::string string) const
        {
            const char* stringCStr = string.c_str();
            uint32_t stringSize = string.length();
            addBytes(byteVec, (void*) &stringSize, sizeof(stringSize));
            addBytes(byteVec, (void*) stringCStr, stringSize);
        }

        uint32_t loadStringBytes(uint8_t* srcPtr, std::string& targetStr)
        {
            uint32_t stringSize;
            loadBytes(srcPtr, &stringSize, sizeof(stringSize));

            std::vector<uint8_t> stringBytes;
            stringBytes.resize(stringSize);
            loadBytes(srcPtr + sizeof(stringSize), &stringBytes[0], stringSize);

            targetStr = std::string(stringBytes.begin(), stringBytes.end());

            return sizeof(stringSize) + stringSize;
        }
    };
}

#endif // MESSAGE_H