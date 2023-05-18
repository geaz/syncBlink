#ifndef OBJECT_H
#define OBJECT_H

#include "program/program_types.hpp"

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

        MAXITEM index = 0;
    };
}

#endif // OBJECT_H