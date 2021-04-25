#include "native_function_object.hpp"

namespace SyncBlink
{
    NativeFunObj::NativeFunObj(NativeFunc nativeFun, uint8_t arity) : _nativeFun(nativeFun), _arity(arity)
    {
    }

    uint8_t NativeFunObj::getArity() const
    {
        return _arity;
    }
    NativeFunc NativeFunObj::getFun() const
    {
        return _nativeFun;
    }
    ObjectType NativeFunObj::getType() const
    {
        return ObjectType::NATIVEFUN;
    }
}