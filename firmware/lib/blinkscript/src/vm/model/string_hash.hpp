#ifndef STRINGHASH_H
#define STRINGHASH_H

#include <string>
#include <cinttypes>

namespace SyncBlink
{
    inline uint32_t stringHash(const std::string& value)
    {
        uint32_t hash = 2166136261u;
        for (int i = 0; i < value.length(); i++) {
            hash ^= (uint8_t)value[i];
            hash *= 16777619;
        }
        return hash;
    }
}

#endif // STRINGHASH_H