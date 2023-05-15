#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>

namespace SyncBlink
{
    enum class ObjectType
    {
        STRING,
        FUN,
        NATIVEFUN,
        ARRAY,
        CLOSURE
    };

    class Object
    {
    public:
        virtual ObjectType getType() const = 0;

        int16_t index = 0;
    };
}

#endif // OBJECT_H