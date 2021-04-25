#ifndef ARRAYOBJECT_H
#define ARRAYOBJECT_H

#include "object.hpp"
#include "blinkscript/vm/model/value.hpp"

#include <vector>

namespace SyncBlink
{
    class ArrayObj : public Object
    {
    public:
        ArrayObj(const std::vector<Value>& arrayValues);

        ObjectType getType() const;
        std::vector<Value>& getValues();

    private:
        std::vector<Value> _arrayValues;
    };
}

#endif // ARRAYOBJECT_H