#include "closure_object.hpp"

namespace SyncBlink
{
    ClosureObj::ClosureObj(FunObj* funPtr, const std::vector<Value>& foreignLocals) 
        : _funPtr(funPtr), _foreignLocals(foreignLocals)
    {
    }

    ObjectType ClosureObj::getType() const
    {
        return ObjectType::CLOSURE;
    }

    const FunObj* ClosureObj::getFunPtr() const
    {
        return _funPtr;
    }

    const std::vector<Value>& ClosureObj::getForeignLocals() const
    {
        return _foreignLocals;
    }
}