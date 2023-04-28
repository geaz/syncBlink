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
        ARRAY
    };

    class Object
    {
    public:
        virtual ObjectType getType() const = 0;
    };
}

#endif // OBJECT_H