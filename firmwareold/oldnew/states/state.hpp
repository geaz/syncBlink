#ifndef STATE_H
#define STATE_H

#include <string>

namespace SyncBlink
{
    class State
    {
        public:
            virtual void loop() = 0;
    };
    
    struct StateChangeEvent
    {
        std::shared_ptr<State> NewState;
    };
}

#endif