#ifndef ARRAYOBJECT_H
#define ARRAYOBJECT_H

#include "object.hpp"
#include "program/model/value.hpp"

#include <vector>

namespace SyncBlink
{
    class ArrayObj : public Object
    {
    public:
        ArrayObj(const std::vector<Value>& arrayValues);

        ObjectType getType() const;
        std::vector<Value>& getValues();
        const std::vector<Value>& getValuesConst() const;

    private:
        std::vector<Value> _arrayValues;
    };
}

#endif // ARRAYOBJECT_H