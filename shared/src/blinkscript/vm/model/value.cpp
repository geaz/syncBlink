#include "value.hpp"

namespace SyncBlink
{
    Value::Value() : _type(ValueType::NIL)
    {
    }

    Value::Value(bool boolean) : _type(ValueType::BOOL)
    {
        this->boolean = boolean;
    }

    Value::Value(float number) : _type(ValueType::NUMBER)
    {
        this->number = number;
    }

    Value::Value(Object* object) : _type(ValueType::OBJECT)
    {
        this->object = object;
    }

    ValueType Value::getType() const
    {
        return _type;
    }
}