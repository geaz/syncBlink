#ifndef SCRIPTCHANGEEVENT_H
#define SCRIPTCHANGEEVENT_H

#include "scripting/script.hpp"

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