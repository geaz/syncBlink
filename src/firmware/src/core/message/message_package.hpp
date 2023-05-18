#ifndef MESSAGEPACKAGE_H
#define MESSAGEPACKAGE_H

#include <cinttypes>
#include <cstring>
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
    };
}

#endif // MESSAGEPACKAGE_H