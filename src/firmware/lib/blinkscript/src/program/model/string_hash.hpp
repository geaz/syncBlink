#ifndef STRINGHASH_H
#define STRINGHASH_H

#include <cinttypes>
#include <string>

namespace SyncBlink
{
    inline uint32_t stringHash(const std::string& value)
    {
        uint32_t hash = 2166136261u;
        for (size_t i = 0; i < value.length(); i++)
        {
            hash ^= (uint8_t)value[i];
            hash *= 16777619;
        }
        return hash;
    }
}

#endif // STRINGHASH_H