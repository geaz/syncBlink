#ifndef CLOSUREOBJ_H
#define CLOSUREOBJ_H

#include "object.hpp"
#include "function_object.hpp"
#include "native_function_object.hpp"
#include "program/model/value.hpp"
#include "program/program.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    class ClosureObj : public Object
    {
    public:
        ClosureObj(FunObj* funPtr, const std::vector<Value>& foreignLocals);

        ObjectType getType() const override;
        const FunObj* getFunPtr() const;

        const std::vector<Value>& getForeignLocals() const;

    private:
        ObjectType _funType;
        FunObj* _funPtr = nullptr;
        std::vector<Value> _foreignLocals;
    };
}

#endif // CLOSUREOBJ_H