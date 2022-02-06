#ifndef MESHUPDATEMSG_H
#define MESHUPDATEMSG_H

#include "core/script.hpp"

#include <cinttypes>

namespace SyncBlink
{
    namespace Messages
    {
        struct MeshUpdate
        {
            Script script;
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;
            uint32_t meshNodeCount;
        };
    }
}

#endif // MESHUPDATEMSG_H