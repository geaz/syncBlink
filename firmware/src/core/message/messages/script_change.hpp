#ifndef SCRIPTCHANGEMSG_H
#define SCRIPTCHANGEMSG_H

#include "core/script.hpp"

namespace SyncBlink
{
    namespace Messages
    {
        struct ScriptChange
        {
            Script script;
        };
    }
}

#endif // SCRIPTCHANGEMSG_H