#ifndef NATIVEFUNOBJ_H
#define NATIVEFUNOBJ_H

#include "object.hpp"
#include "vm/frame.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    typedef std::function<Value(Frame&)> NativeFunc;

    class NativeFunObj : public Object
    {
    public:
        NativeFunObj(NativeFunc nativeFun, uint8_t arity);

        uint8_t getArity() const;
        NativeFunc getFun() const;
        ObjectType getType() const;

    private:
        NativeFunc _nativeFun;
        uint8_t _arity;
    };
}

#endif // NATIVEFUNOBJ_H