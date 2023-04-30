#ifndef MESSAGE_H
#define MESSAGE_H

#include "message_package.hpp"
#include "message_types.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace SyncBlink
{
    class Message
    {
    public:
        std::vector<uint8_t> toPackage() const
        {
            auto packageBody = getPackageBody();
            size_t bodySize = packageBody.size();
            size_t packageSize = bodySize + PacketHeaderSize;

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

        virtual std::vector<uint8_t> getPackageBody() const = 0;
        virtual void loadPackage(MessagePackage package)  = 0;
        virtual MessageType getMessageType() const  = 0;

        uint64_t recipientId = 0;

    protected:
        void addBytes(std::vector<uint8_t>& byteVec, const void* dataPtr, size_t dataSize) const
        {
            size_t curSize = byteVec.size();
            byteVec.resize(curSize + dataSize);

            memcpy(&byteVec[curSize], dataPtr, dataSize);
        }

        size_t loadBytes(void* srcPtr, void* targetPtr, size_t targetSize)
        {
            memcpy(targetPtr, srcPtr, targetSize);
            return targetSize;
        }

        void addStringBytes(std::vector<uint8_t>& byteVec, std::string string) const
        {
            const char* stringCStr = string.c_str();
            size_t stringSize = string.length();
            addBytes(byteVec, (void*)&stringSize, sizeof(stringSize));
            addBytes(byteVec, (void*)stringCStr, stringSize);
        }

        size_t loadStringBytes(uint8_t* srcPtr, std::string& targetStr)
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