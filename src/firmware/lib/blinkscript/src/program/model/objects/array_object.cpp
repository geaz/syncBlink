#include "array_object.hpp"

namespace SyncBlink
{
    ArrayObj::ArrayObj(const std::vector<Value>& arrayValues) : _arrayValues(arrayValues)
    {
    }

    ObjectType ArrayObj::getType() const
    {
        return ObjectType::ARRAY;
    }

    std::vector<Value>& ArrayObj::getValues()
    {
        return _arrayValues;
    }

    const std::vector<Value>& ArrayObj::getValuesConst() const
    {
        return _arrayValues;
    }
}