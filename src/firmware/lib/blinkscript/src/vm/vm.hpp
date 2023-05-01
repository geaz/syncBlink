#ifndef VM_H
#define VM_H

#include "model/value.hpp"
#include "model/frame.hpp"
#include "model/program.hpp"
#include "model/objects/array_object.hpp"
#include "model/objects/string_object.hpp"
#include "model/objects/function_object.hpp"
#include "model/objects/native_function_object.hpp"

#include <map>
#include <vector>
#include <memory>

namespace SyncBlink
{
    class VM
    {
    public:
        void run(const Program& program);

        void addNativeFun(const std::string& identifier, std::shared_ptr<NativeFunObj> nativeFun);
        void executeFun(const std::string& identifier, std::vector<Value> parameters);

        Frame& getFrame() const;
        const Value& getTop() const;
        std::tuple<int, std::string> getError() const;
        const bool hasError() const;

    private:
        void handleSet(const Program& program, size_t& i, bool define);
        void handleLoad(const Program& program, size_t& i);
        void handleJump(const Program& program, size_t& i);
        void handleJumpNot(const Program& program, size_t& i);
        void handleCall(const Program& program, size_t& i);
        void handlePrefix(const Program& program, size_t i);
        void handleInfix(const Program& program, size_t i);
        void handleIndex(const Program& program, size_t i, bool set = false);

        void executeFun(uint32_t hash, int line, std::vector<Value> parameters, bool paramFromStack);
        StringObj* getStringObjectValue(const Program& program, int i);
        Value popValue();
        
        std::vector<Value> _stack;
        std::vector<std::shared_ptr<Object>> _nativeFuns;
        std::vector<std::shared_ptr<Object>> _runTimeObjects;
        std::shared_ptr<Frame> _frame = std::make_shared<Frame>(); 
        std::shared_ptr<Frame> _callFrame = nullptr;
        std::tuple<int, std::string> _vmError = std::make_tuple(-99, "");
    };
}

#endif // VM_H