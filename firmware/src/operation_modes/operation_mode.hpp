#ifndef OPERATIONMODE_H
#define OPERATIONMODE_H

namespace SyncBlink
{
    class OperationMode
    {
        public:
            virtual void setup() = 0;
            virtual void loop() = 0;
    };
}

#endif // OPERATIONMODE_H