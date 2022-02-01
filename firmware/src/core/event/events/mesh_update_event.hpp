#ifndef MESHUPDATEEVENT_H
#define MESHUPDATEEVENT_H

#include <cinttypes>

namespace SyncBlink
{
    namespace Events
    {
        struct MeshUpdateEvent
        {
            uint32_t routeLedCount;
            uint32_t routeNodeCount;
            uint32_t meshLedCount;
            uint32_t meshNodeCount;
        };
    }
}

#endif // MESHUPDATEEVENT_H