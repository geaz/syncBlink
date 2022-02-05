#ifndef SCRIPTCHANGEEVENT_H
#define SCRIPTCHANGEEVENT_H

#include "core/script.hpp"

namespace SyncBlink
{
    namespace Events
    {
        struct ScriptChangeEvent
        {
            Script script;
        };
    }
}

#endif // SCRIPTCHANGEEVENT_H