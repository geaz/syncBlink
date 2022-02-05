#ifndef MODULE_H
#define MODULE_H

namespace SyncBlink
{
    class Module
    {
    public:
        virtual void setup() { };
        virtual void loop() { };
    };
}

#endif // MODULE_H