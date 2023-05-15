#ifndef NATIVEFUNOBJ_H
#define NATIVEFUNOBJ_H

#include "object.hpp"
#include "vm/symbol_table.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    typedef std::function<Value(std::vector<Value>&)> NativeFunc;

    class NativeFunObj : public Object
    {
    public:
        NativeFunObj(NativeFunc nativeFun, size_t arity);

        size_t getArity() const;
        NativeFunc getFun() const;
        ObjectType getType() const;

    private:
        NativeFunc _nativeFun;
        size_t _arity;
    };
}

#endif // NATIVEFUNOBJ_H