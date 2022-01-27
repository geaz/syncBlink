#ifndef SCRIPTBUILTINS_H
#define SCRIPTBUILTINS_H

#include "vm/vm.hpp"
#include <led.hpp>

namespace SyncBlink
{
    class BlinkScript;
    namespace BuiltIns
    {
        void println(VM& vm);
        void setGroups(BlinkScript& blinkScript, VM& vm);
        void setLinearGroups(BlinkScript& blinkScript, VM& vm);
        void clearGroups(BlinkScript& blinkScript, VM& vm);
        void setDelay(BlinkScript& blinkScript, VM& vm);
        void getLed(BlinkScript& blinkScript, VM& vm);
        void setLeds(BlinkScript& blinkScript, VM& vm);
        void setAllLeds(BlinkScript& blinkScript, VM& vm);
        void map(VM& vm);
        void xrgb(VM& vm);
        void xhsv(VM& vm);
    };
}

#endif // SCRIPTBUILDINS_H