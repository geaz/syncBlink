#ifndef STATE_H
#define STATE_H

#include <string>

namespace SyncBlink
{
    class StationContext;
    class State
    {
        public:
            virtual void run(StationContext& context) = 0;
    };
}

#endif