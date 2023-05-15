#include "native_function_object.hpp"

namespace SyncBlink
{
    NativeFunObj::NativeFunObj(NativeFunc nativeFun, size_t arity) : _nativeFun(nativeFun), _arity(arity)
    {
    }

    size_t NativeFunObj::getArity() const
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