#ifndef VALUE_H
#define VALUE_H

#include "objects/object.hpp"

#include <string>

namespace SyncBlink
{
    enum class ValueType { NIL, BOOL, NUMBER, OBJECT };

    class Value
    {
    public:
        Value();
        explicit Value(bool boolean);
        explicit Value(float number);
        explicit Value(Object* object);

        ValueType getType() const;

        template<class T>
        T* getObject()
        {
            return static_cast<T*>(this->object);
        }

        union {
            bool boolean = false;
            float number;
            Object* object;
        };

    private:
        ValueType _type;
    };
}

#endif // VALUE_H