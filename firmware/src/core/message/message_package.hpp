#ifndef MESSAGEPACKAGE_H
#define MESSAGEPACKAGE_H

#include <Arduino.h>
#include <Stream.h>
#include <vector>

namespace SyncBlink
{
    static const uint8_t SocketWriteTimeout = 100;
    static const char PacketMagicBytes[3] = {'S', 'B', 'M'};
    static const uint8_t PacketHeaderSize = 4 /* package size info */ + 3 /* magic bytes = 'SBM' */ + 1 /* message type */;

    class MessagePackage
    {
    public:
        uint8_t type;
        std::vector<uint8_t> body;

        static bool available(Stream& stream, MessagePackage& message)
        {
            bool receivedMessage = false;
            while (stream.available())
            {
                uint8_t magicBuf[2];
                uint8_t byte = stream.read();
                if (byte == PacketMagicBytes[0])
                {
                    magicBuf[0] = stream.read();
                    magicBuf[1] = stream.read();
                    if (magicBuf[0] == PacketMagicBytes[1] && magicBuf[1] == PacketMagicBytes[2])
                    {
                        uint32_t messageSize = (stream.read() << 24) + (stream.read() << 16) + (stream.read() << 8) + stream.read();
                        uint8_t messageType = stream.read();

                        message.type = messageType;
                        message.body.resize(messageSize);

                        for (uint32_t byte = 0; byte < messageSize; byte++)
                        {
                            while (!stream.available())
                                yield();                        // Wait for new bytes
                            message.body[byte] = stream.read(); // then read
                        }

                        receivedMessage = true;
                        break;
                    }
                }
            }
            return receivedMessage;
        }
    };
}

#endif // MESSAGEPACKAGE_H