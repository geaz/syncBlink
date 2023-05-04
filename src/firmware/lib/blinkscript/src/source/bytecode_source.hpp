#ifndef BYTECODESOURCE_H
#define BYTECODESOURCE_H

#include <string>
#include <fstream>

namespace SyncBlink
{
    class ByteCodeSource
    {
    public:
        virtual uint8_t getByte(const size_t pos) = 0;
    };
}

#endif // BYTECODESOURCE_H