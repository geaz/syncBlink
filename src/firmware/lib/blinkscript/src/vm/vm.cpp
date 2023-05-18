#include "vm.hpp"

#include "program/model/op_codes.hpp"
#include "program/model/string_hash.hpp"

#include <sstream>
#include <stdio.h>
#include <algorithm>

namespace SyncBlink
{
    void VM::run(const Program& program)
    {
        for (MAXCODE ip = 0; ip < program.getCode().size(); ip++)
        {
            auto& code = program.getCode()[ip];
            switch (code)
            {
            case OpCode::POP:
                _stack.pop_back();
                break;
            case OpCode::VALUE:
                _stack.push_back(program.getConstants()[program.getCode()[++ip]]);
                break;
            case OpCode::CLOSURE:
                handleClosureLoad(program, ip);
                break;
            case OpCode::DEFINE_GLOBAL:
                handleDefineGlobal(program, ip);
                break;
            case OpCode::SET_GLOBAL:
                handleSet(program, ip, false);
                break;
            case OpCode::SET_LOCAL:
                handleSet(program, ip, true);
                break;
                break;
            case OpCode::SET_INDEX:
                handleIndex(program, ip, true);
                break;
            case OpCode::LOAD_GLOBAL:
                handleLoad(program, ip, false);
                break;
            case OpCode::LOAD_LOCAL:
                handleLoad(program, ip, true);
                break;
                break;
            case OpCode::INDEX:
                handleIndex(program, ip);
                break;
            case OpCode::JMP:
                handleJump(program, ip);
                break;
            case OpCode::JMP_NOT:
                handleJumpNot(program, ip);
                break;
            case OpCode::CALL:
                executeFun(popValue(), program.getLines()[ip - 1]);
                break;
            case OpCode::CLEAR_SCOPE:
                handleClearScope(program, ip);
                break;
            case OpCode::NEGATE:
            case OpCode::BANG_NEGATE:
                handlePrefix(program, ip);
                break;
            case OpCode::ADD:
            case OpCode::SUB:
            case OpCode::MUL:
            case OpCode::DIV:
            case OpCode::MODULO:
            case OpCode::AND:
            case OpCode::OR:
            case OpCode::LESS:
            case OpCode::LESS_EQUAL:
            case OpCode::GREATER:
            case OpCode::GREATER_EQUAL:
            case OpCode::EQUAL_EQUAL:
            case OpCode::BANG_EQUAL:
                handleInfix(program, ip);
                break;
            }
            if (hasError()) return;
        }
    }

    void VM::addNativeFun(const std::string& identifier, std::shared_ptr<NativeFunObj> nativeFun)
    {
        _nativeFuns.push_back(nativeFun);
        _globalVariables.set(identifier, Value(nativeFun.get()));
    }

    void VM::executeFun(const std::string& identifier, std::vector<Value> parameters)
    {
        Value fun;
        if(!_globalVariables.get(identifier, &fun))
        {
            _vmError = std::make_tuple(0, "VM: Function not found (executeFun)!");
            return;
        }
        if (fun.getType() != ValueType::OBJECT && fun.object->getType() != ObjectType::FUN && fun.object->getType() != ObjectType::NATIVEFUN)
        {
            _vmError = std::make_tuple(0, "VM: Variable is not a function (executeFun)!");
            return;
        }

        _stack.clear();

        // Remove not used runtime object
        // Stack is clear at this state. We just have to check global variables.
        for(auto iterRuntime = _runTimeObjects.begin(); iterRuntime != _runTimeObjects.end();)
        {
            if (!_globalVariables.hasRefOnObject(iterRuntime->get())) iterRuntime = _runTimeObjects.erase(iterRuntime);
            else ++iterRuntime;
        }

        for(Value parameter : parameters)
        {
            _stack.push_back(parameter);
        }

        executeFun(fun, 0);
    }

    void VM::setGlobal(const std::string& identifier, Value value)
    {
        _globalVariables.set(identifier, value);
    }

    Value VM::getTop() const
    {
        if (_stack.size() > 0) return _stack.back();
        return Value();
    }

    size_t VM::getStackSize() const
    {
        return _stack.size();
    }

    std::tuple<LINETYPE, std::string> VM::getError() const
    {
        return _vmError;
    }

    const bool VM::hasError() const
    {
        return std::get<1>(_vmError) != "";
    }

    void VM::handleClosureLoad(const Program& program, MAXCODE& ip)
    {
        auto funValue = program.getConstants()[program.getCode()[++ip]];
        if(funValue.getType() != ValueType::OBJECT && funValue.object->getType() != ObjectType::FUN)
        {
            _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Unexpected error! Object not a function!");
            return;
        }
        auto funObj = funValue.getObject<FunObj>();
        
        std::vector<Value> foreignValues;
        for(size_t index : funObj->getForeignLocalIndices())
        {
            foreignValues.push_back(_stack.at(index));
        }

        auto closureObj = std::make_shared<ClosureObj>(funObj, foreignValues);        
        _runTimeObjects.push_back(closureObj);        
        _stack.push_back(Value(closureObj.get()));
    }   
    
    void VM::handleClearScope(const Program& program, MAXCODE& ip)
    {
        // all scopes do return a value - we have to preserve it and then pop all local values from the stack
        // after that we push the return value back
        Value returnValue = popValue();
        size_t localPopCount = (size_t)program.getConstants()[program.getCode()[++ip]].number;
        for (size_t i = 0; i < localPopCount; i++)
            _stack.pop_back();
        _stack.push_back(returnValue);
    }

    void VM::handleDefineGlobal(const Program& program, MAXCODE& ip)
    {
        auto strObj = getStringObjectValue(program, ++ip);
        if (strObj != nullptr)
        {
            if(_globalVariables.set(strObj->getHash(), _stack.back())) _stack.pop_back();
            else _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Unexpected error! Variable already defined!");
        }
        else _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Unexpected error! Object not found!");
    }

    void VM::handleSet(const Program& program, MAXCODE& ip, bool local)
    {
        auto opParam = program.getConstants()[program.getCode()[++ip]];
        if(local)  
        {
            if(opParam.getType() != ValueType::NUMBER)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Expected number for local index!");
                return;
            }
            if(_stack.size() - 1 < opParam.number)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Local index out of range!");
                return;
            }
            if (_stackFunOffsets.size() > 0) _stack.at(_stackFunOffsets.back() + (size_t)opParam.number) = _stack.back();
            else _stack.at((size_t)opParam.number) = _stack.back();
        }
        else
        {
            if (opParam.getType() != ValueType::OBJECT || opParam.object->getType() != ObjectType::STRING)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Expected string for global var!");
                return;
            }
            StringObj* strObj = static_cast<StringObj*>(opParam.object);
            if (_globalVariables.set(strObj->getHash(), _stack.back()))
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Unexpected error! Variable is not defined!");
        }
    }

    void VM::handleLoad(const Program& program, MAXCODE& ip, bool local)
    {
        auto opParam = program.getConstants()[program.getCode()[++ip]];
        if(local)  
        {
            if(opParam.getType() != ValueType::NUMBER)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Expected number for local index!");
                return;
            }
            if(_stack.size() - 1 < opParam.number)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Local index out of range!");
                return;
            }
            if (_stackFunOffsets.size() > 0) _stack.push_back(_stack.at(_stackFunOffsets.back() + (size_t)opParam.number));
            else _stack.push_back(_stack.at((size_t)opParam.number));
        }
        else
        {
            if (opParam.getType() != ValueType::OBJECT || opParam.object->getType() != ObjectType::STRING)
            {
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Expected string for global var!");
                return;
            }
            StringObj* strObj = static_cast<StringObj*>(opParam.object);
            Value value;
            if (!_globalVariables.get(strObj->getHash(), &value))
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Variable not defined!");
            else  _stack.push_back(value);
        }
    }

    void VM::handleJump(const Program& program, MAXCODE& ip)
    {
        ip += 2; // Next Instruction is VALUE, after this the actual jump value
        auto jmpValue = program.getConstants()[program.getCode()[ip]];
        if (jmpValue.getType() != ValueType::NUMBER) _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: JMP value not a number!");
        else
            ip = (uint16_t)jmpValue.number - 1; // decrease by one because of for loop increment
    }

    void VM::handleJumpNot(const Program& program, MAXCODE& ip)
    {
        auto value = popValue();
        if (value.getType() != ValueType::BOOL) _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Condition value not a boolean!");
        else if (!value.boolean)
        {
            ip += 2; // Next Instruction is VALUE, after this the actual jump value
            auto jmpValue = program.getConstants()[program.getCode()[ip]];
            if (jmpValue.getType() != ValueType::NUMBER)
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: JMP value not a number!");
            else
                ip = (uint16_t)jmpValue.number - 1; // decrease by one because of for loop increment
        }
        else
            ip += 2; // Jump over jump value
    }

    void VM::handleReturn(const Program& program, MAXCODE& ip)
    {
        auto opParam = program.getConstants()[program.getCode()[++ip]];
        if (opParam.getType() != ValueType::NUMBER) 
        {
            _vmError = std::make_tuple(program.getLines()[ip], "VM: Value on stack is not a number (return)!");
            return;
        }
        // save return value
        Value returnValue = popValue();
        // POP local values
        for(size_t i = 0; i < (size_t)opParam.number; i++)
            popValue();
        // Push back return value
        _stack.push_back(returnValue);
    }

    void VM::handlePrefix(const Program& program, MAXCODE ip)
    {
        auto& code = program.getCode()[ip];
        auto value = popValue();
        if (value.getType() == ValueType::BOOL && code == OpCode::BANG_NEGATE)
        {
            value.boolean = !value.boolean;
            _stack.push_back(value);
        }
        else if (value.getType() == ValueType::NUMBER && code == OpCode::NEGATE)
        {
            value.number = -value.number;
            _stack.push_back(value);
        }
        else
            _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid prefix operator!");
    }

    void VM::handleInfix(const Program& program, MAXCODE ip)
    {
        const CODETYPE& code = program.getCode()[ip];
        auto rightValue = popValue();
        auto leftValue = popValue();

        if (leftValue.getType() == ValueType::BOOL && rightValue.getType() == ValueType::BOOL)
        {
            switch (code)
            {
            case OpCode::AND:
                leftValue.boolean = leftValue.boolean && rightValue.boolean;
                _stack.push_back(leftValue);
                break;
            case OpCode::OR:
                leftValue.boolean = leftValue.boolean || rightValue.boolean;
                _stack.push_back(leftValue);
                break;
            case OpCode::EQUAL_EQUAL:
                leftValue.boolean = leftValue.boolean == rightValue.boolean;
                _stack.push_back(leftValue);
                break;
            case OpCode::BANG_EQUAL:
                leftValue.boolean = leftValue.boolean != rightValue.boolean;
                _stack.push_back(leftValue);
                break;
            default:
                _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid infix operator for boolean!");
                break;
            }
        }
        else if (leftValue.getType() == ValueType::NUMBER && rightValue.getType() == ValueType::NUMBER)
        {
            switch (code)
            {
            case OpCode::ADD:
                leftValue.number = leftValue.number + rightValue.number;
                _stack.push_back(leftValue);
                break;
            case OpCode::SUB:
                leftValue.number = leftValue.number - rightValue.number;
                _stack.push_back(leftValue);
                break;
            case OpCode::DIV:
                leftValue.number = leftValue.number / rightValue.number;
                _stack.push_back(leftValue);
                break;
            case OpCode::MUL:
                leftValue.number = leftValue.number * rightValue.number;
                _stack.push_back(leftValue);
                break;
            case OpCode::MODULO:
                leftValue.number = std::fmod(leftValue.number, rightValue.number);
                _stack.push_back(leftValue);
                break;
            case OpCode::LESS: {
                Value value(leftValue.number < rightValue.number);
                _stack.push_back(value);
                break;
            }
            case OpCode::LESS_EQUAL: {
                Value value(leftValue.number <= rightValue.number);
                _stack.push_back(value);
                break;
            }
            case OpCode::GREATER: {
                Value value(leftValue.number > rightValue.number);
                _stack.push_back(value);
                break;
            }
            case OpCode::GREATER_EQUAL: {
                Value value(leftValue.number >= rightValue.number);
                _stack.push_back(value);
                break;
            }
            case OpCode::EQUAL_EQUAL: {
                Value value(leftValue.number == rightValue.number);
                _stack.push_back(value);
                break;
            }
            case OpCode::BANG_EQUAL: {
                Value value(leftValue.number != rightValue.number);
                _stack.push_back(value);
                break;
            }
            default:
                _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid infix operator for number!");
                break;
            }
        }
        else if (leftValue.getType() == ValueType::OBJECT && rightValue.getType() == ValueType::OBJECT)
        {
            if (leftValue.object->getType() == ObjectType::STRING && rightValue.object->getType() == ObjectType::STRING)
            {
                auto leftString = static_cast<StringObj*>(leftValue.object);
                auto rightString = static_cast<StringObj*>(rightValue.object);
                switch (code)
                {
                case OpCode::ADD: {
                    auto resultString = std::make_shared<StringObj>(leftString->getString() + rightString->getString());
                    _runTimeObjects.push_back(resultString);
                    Value value(resultString.get());

                    _stack.push_back(value);
                    break;
                }
                case OpCode::EQUAL_EQUAL: {
                    Value value(leftString->getString() == rightString->getString());
                    _stack.push_back(value);
                    break;
                }
                case OpCode::BANG_EQUAL: {
                    Value value(leftString->getString() != rightString->getString());
                    _stack.push_back(value);
                    break;
                }
                default:
                    _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid infix operator for string!");
                    break;
                }
            }
            else
                _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid infix operator for objects!");
        }
        else if (leftValue.getType() == ValueType::OBJECT && leftValue.object->getType() == ObjectType::STRING &&
                 rightValue.getType() == ValueType::NUMBER)
        {
            auto leftString = static_cast<StringObj*>(leftValue.object);
            std::stringstream numberString;
            switch (code)
            {
            case OpCode::ADD: {
                numberString << rightValue.number;
                auto resultString = std::make_shared<StringObj>(leftString->getString() + numberString.str());
                _runTimeObjects.push_back(resultString);
                Value value(resultString.get());

                _stack.push_back(value);
                break;
            }
            default:
                _vmError =
                    std::make_tuple(program.getLines()[ip], "VM: Invalid infix parameter! Its only possible to 'add' numbers and strings!");
                break;
            }
        }
        else
            _vmError = std::make_tuple(program.getLines()[ip], "VM: Invalid infix parameter combination!");
    }

    void VM::handleIndex(const Program& program, MAXCODE ip, bool set)
    {
        auto indexValue = popValue();
        if (indexValue.getType() != ValueType::NUMBER) _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Index must be a number!");
        else
        {
            auto arrayValue = popValue();
            if (arrayValue.getType() != ValueType::OBJECT || arrayValue.object->getType() != ObjectType::ARRAY)
                _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Only arrays support indexing!");
            else
            {
                auto arrayObj = static_cast<ArrayObj*>(arrayValue.object);
                if (indexValue.number + 1 > arrayObj->getValues().size()) arrayObj->getValues().resize((MAXITEM)indexValue.number + 1);

                if (set) arrayObj->getValues()[(MAXITEM)indexValue.number] = _stack.back();
                else
                    _stack.push_back(arrayObj->getValues()[(MAXITEM)indexValue.number]);
            }
        }
    }

    void VM::executeFun(Value value, LINETYPE line)
    {
        const ClosureObj* closureObj = nullptr;
        const NativeFunObj* nativeFunObj = nullptr;

        if (value.getType() != ValueType::OBJECT && value.object->getType() != ObjectType::CLOSURE &&
            value.object->getType() != ObjectType::NATIVEFUN) 
        {
            _vmError = std::make_tuple(line, "VM: Value does not hold a native function or closure!");
            return;
        }

        if (value.object->getType() == ObjectType::CLOSURE)
        {
            const ClosureObj* closureObj = value.getObject<ClosureObj>();
            for(auto foreignLocal : closureObj->getForeignLocals())
            {
                _stack.push_back(foreignLocal);
            }

            _stackFunOffsets.push_back(_stack.size() 
                - closureObj->getForeignLocals().size() 
                - closureObj->getFunPtr()->getParameters().size());
            run(closureObj->getFunPtr()->getProgram());
            _stackFunOffsets.pop_back();
        }
        else if (value.object->getType() == ObjectType::NATIVEFUN)
        {
            const NativeFunObj* nativeFunObj = value.getObject<NativeFunObj>();
            std::vector<Value> parameters;
            for (size_t i = 0; i < nativeFunObj->getArity(); i++)
            {
                parameters.push_back(_stack.back());
                _stack.pop_back();
            }
            // reverse parameter vector to get them in the right order for the native func
            std::reverse(parameters.begin(), parameters.end());
            _stack.push_back(nativeFunObj->getFun()(parameters));
        }
    }

    StringObj* VM::getStringObjectValue(const Program& program, MAXCODE ip)
    {
        auto& value = program.getConstants()[program.getCode()[ip]];
        if (value.getType() == ValueType::OBJECT && value.object->getType() == ObjectType::STRING)
            return static_cast<StringObj*>(value.object);
        else
        {
            _vmError = std::make_tuple(program.getLines()[ip - 1], "VM: Variable name is not a string object!");
            return nullptr;
        }
    }

    Value VM::popValue()
    {
        auto value = _stack.back();
        _stack.pop_back();
        return value;
    }
}
