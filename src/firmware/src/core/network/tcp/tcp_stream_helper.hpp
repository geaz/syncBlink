#ifndef TCPSTREAMHELPER_H
#define TCPSTREAMHELPER_H

#include "core/message/message_package.hpp"

#include <Arduino.h>
#include <Stream.h>

namespace SyncBlink
{
    class TcpStreamHelper
    {
    public:
        static bool messageAvailable(Stream& stream, MessagePackage& message)
        {
            uint8_t magicBuf[2];
            bool receivedMessage = false;
            while (stream.available())
            {
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

#endif // TCPSTREAMHELPER_H