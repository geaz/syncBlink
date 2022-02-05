#ifndef MESSAGE_H
#define MESSAGE_H

#include "core/event/event_bus.hpp"

#include <Arduino.h>
#include <Stream.h>
#include <vector>

namespace SyncBlink
{
    static const uint8_t SocketWriteTimeout = 100;
    static const char SocketMagicBytes[3] = {'S', 'B', 'M'};
    static const uint8_t SocketHeaderSize =
        4 /* package size info */ + 3 /* magic bytes = 'SBM' */ + 1 /* message type */;

    class Message
    {
    public:
        uint8_t type;
        std::vector<uint8_t> body;

        template <class T> T as()
        {
            T typedMessage;
            memcpy(&typedMessage, &body[0], body.size());
            return typedMessage;
        }

        static std::vector<uint8_t> toMessagePacket(const void* body, uint32_t bodySize, uint8_t eventType)
        {
            uint32_t packageSize = bodySize + SocketHeaderSize;

            std::vector<uint8_t> package;
            package.resize(packageSize);

            package[0] = SocketMagicBytes[0];
            package[1] = SocketMagicBytes[1];
            package[2] = SocketMagicBytes[2];
            package[3] = (uint8_t)(bodySize >> 24);
            package[4] = (uint8_t)(bodySize >> 16);
            package[5] = (uint8_t)(bodySize >> 8);
            package[6] = (uint8_t)(bodySize >> 0);
            package[7] = eventType;
            if (bodySize > 0) memcpy(&package[8], body, bodySize);

            return package;
        }

        static bool available(Stream& stream, Message& message)
        {
            bool receivedMessage = false;
            while (stream.available())
            {
                uint8_t magicBuf[2];
                uint8_t byte = stream.read();
                if (byte == SocketMagicBytes[0])
                {
                    magicBuf[0] = stream.read();
                    magicBuf[1] = stream.read();
                    if (magicBuf[0] == SocketMagicBytes[1] && magicBuf[1] == SocketMagicBytes[2])
                    {
                        uint32_t messageSize =
                            (stream.read() << 24) + (stream.read() << 16) + (stream.read() << 8) + stream.read();
                        uint8_t messageType = stream.read();

                        uint8_t checksum = 0;
                        checksum += SocketMagicBytes[0] % 2;
                        checksum += SocketMagicBytes[1] % 2;
                        checksum += SocketMagicBytes[2] % 2;
                        checksum += (uint8_t)(messageSize >> 24) % 2;
                        checksum += (uint8_t)(messageSize >> 16) % 2;
                        checksum += (uint8_t)(messageSize >> 8) % 2;
                        checksum += (uint8_t)(messageSize >> 0) % 2;

                        message.type = messageType;
                        message.body.resize(messageSize);

                        for (uint32_t byte = 0; byte < messageSize; byte++)
                        {
                            while (!stream.available())
                                yield();                        // Wait for new bytes
                            message.body[byte] = stream.read(); // then read
                        }

#ifdef DEBUG_TCP
                        Serial.printf("[STREAM] Found message - Size: %i, Type: %i\n",
                                      message.body.size() + SocketHeaderSize, message.type);
#endif

                        receivedMessage = true;
                        break;
                    }
                }
            }
            return receivedMessage;
        }
    };
}

#endif // MESSAGE_H