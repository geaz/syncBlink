#ifndef VM_H
#define VM_H

#include "program/model/objects/array_object.hpp"
#include "program/model/objects/function_object.hpp"
#include "program/model/objects/native_function_object.hpp"
#include "program/model/objects/closure_object.hpp"
#include "program/model/objects/string_object.hpp"
#include "program/model/value.hpp"
#include "program/program.hpp"
#include "symbol_table.hpp"

#include <map>
#include <memory>
#include <vector>

namespace SyncBlink
{
    class VM
    {
    public:
        void run(const Program& program);

        void addNativeFun(const std::string& identifier, std::shared_ptr<NativeFunObj> nativeFun);
        void executeFun(const std::string& identifier, std::vector<Value> parameters);

        void setGlobal(const std::string& identifier, Value value);

        Value getTop() const;
        size_t getStackSize() const;
        std::tuple<LINETYPE, std::string> getError() const;

        const bool hasError() const;

    private:
        void handleClosureLoad(const Program& program, MAXCODE& ip);
        void handleClearScope(const Program& program, MAXCODE& ip);
        void handleDefineGlobal(const Program& program, MAXCODE& ip);
        void handleSet(const Program& program, MAXCODE& ip, bool local);
        void handleLoad(const Program& program, MAXCODE& ip, bool local);
        void handleJump(const Program& program, MAXCODE& ip);
        void handleJumpNot(const Program& program, MAXCODE& ip);
        void handleReturn(const Program& program, MAXCODE& ip);
        void handlePrefix(const Program& program, MAXCODE ip);
        void handleInfix(const Program& program, MAXCODE ip);
        void handleIndex(const Program& program, MAXCODE ip, bool set = false);

        void executeFun(Value value, LINETYPE line);
        StringObj* getStringObjectValue(const Program& program, MAXCODE ip);
        Value popValue();

        std::vector<Value> _stack;
        SymbolTable _globalVariables;
        std::vector<size_t> _stackFunOffsets; 

        std::vector<std::shared_ptr<Object>> _nativeFuns;
        std::vector<std::shared_ptr<Object>> _runTimeObjects;
        std::tuple<LINETYPE, std::string> _vmError = std::make_tuple(0, "");
    };
}

#endif // VM_H