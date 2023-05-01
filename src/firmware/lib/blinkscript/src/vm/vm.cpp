#include "vm.hpp"

#include "model/string_hash.hpp"
#include "op_codes.hpp"

#include <sstream>
#include <stdio.h>

namespace SyncBlink
{
    void VM::run(const Program& program)
    {
        for (size_t i = 0; i < program.getCode().size(); i++)
        {
            auto& code = program.getCode()[i];
            switch (code)
            {
            case OpCode::VALUE:
                _stack.push_back(program.getConstant(program.getCode()[++i]));
                break;
            case OpCode::DEFINE:
                handleSet(program, i, true);
                break;
            case OpCode::SET:
                handleSet(program, i, false);
                break;
            case OpCode::SET_INDEX:
                handleIndex(program, i, true);
                break;
            case OpCode::LOAD:
                handleLoad(program, i);
                break;
            case OpCode::INDEX:
                handleIndex(program, i);
                break;
            case OpCode::JMP:
                handleJump(program, i);
                break;
            case OpCode::JMP_NOT:
                handleJumpNot(program, i);
                break;
            case OpCode::FRAME:
                if (_callFrame != nullptr)
                {
                    _frame = _callFrame;
                    _callFrame = nullptr;
                }
                else
                    _frame = std::make_shared<Frame>(_frame);
                break;
            case OpCode::UNFRAME:
                _frame = _frame->getParentFrame();
                break;
            case OpCode::CALL:
                handleCall(program, i);
                break;
            case OpCode::NEGATE:
            case OpCode::BANG_NEGATE:
                handlePrefix(program, i);
                break;
            case OpCode::ADD:
            case OpCode::SUB:
            case OpCode::MUL:
            case OpCode::DIV:
            case OpCode::AND:
            case OpCode::OR:
            case OpCode::LESS:
            case OpCode::LESS_EQUAL:
            case OpCode::GREATER:
            case OpCode::GREATER_EQUAL:
            case OpCode::EQUAL_EQUAL:
            case OpCode::BANG_EQUAL:
                handleInfix(program, i);
                break;
            }
            if (hasError())
                return;
        }
    }

    void VM::addNativeFun(const std::string& identifier, std::shared_ptr<NativeFunObj> nativeFun)
    {
        _nativeFuns.push_back(nativeFun);
        _frame->addSet(identifier, Value(nativeFun.get()));
    }

    void VM::executeFun(const std::string& identifier, std::vector<Value> parameters)
    {
        _stack.clear();
        _runTimeObjects.clear();
        executeFun(stringHash(identifier), -1, parameters, false);
    }

    Frame& VM::getFrame() const
    {
        return *_frame;
    }
    const Value& VM::getTop() const
    {
        return _stack.back();
    }
    std::tuple<int, std::string> VM::getError() const
    {
        return _vmError;
    }
    const bool VM::hasError() const
    {
        return std::get<0>(_vmError) != -99;
    }

    void VM::handleSet(const Program& program, size_t& i, bool define)
    {
        auto strObj = getStringObjectValue(program, ++i);
        if (strObj != nullptr)
        {
            if (define)
            {
                if (_frame->add(strObj->getHash(), _stack.back()))
                    _stack.pop_back();
                else
                    _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Variable already defined!");
            }
            else
            {
                if (_frame->set(strObj->getHash(), _stack.back()))
                    _stack.pop_back();
                else
                    _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Variable not defined!");
            }
        }
    }

    void VM::handleLoad(const Program& program, size_t& i)
    {
        auto strObj = getStringObjectValue(program, ++i);
        if (strObj != nullptr)
        {
            Value value = _frame->get(strObj->getHash());
            if (value.getType() == ValueType::NIL)
                _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Variable not defined!");
            else
                _stack.push_back(value);
        }
    }

    void VM::handleJump(const Program& program, size_t& i)
    {
        i += 2; // Next Instruction is VALUE, after this the actual jump value
        auto jmpValue = program.getConstant(program.getCode()[i]);
        if (jmpValue.getType() != ValueType::NUMBER)
            _vmError = std::make_tuple(program.getLines()[i - 1], "VM: JMP value not a number!");
        else
            i = jmpValue.number - 1; // decrease by one because of for loop increment
    }

    void VM::handleJumpNot(const Program& program, size_t& i)
    {
        auto value = popValue();
        if (value.getType() != ValueType::BOOL)
            _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Condition value not a boolean!");
        else if (!value.boolean)
        {
            i += 2; // Next Instruction is VALUE, after this the actual jump value
            auto jmpValue = program.getConstant(program.getCode()[i]);
            if (jmpValue.getType() != ValueType::NUMBER)
                _vmError = std::make_tuple(program.getLines()[i - 1], "VM: JMP value not a number!");
            else
                i = jmpValue.number - 1; // decrease by one because of for loop increment
        }
        else
            i += 2; // Jump over jump value
    }

    void VM::handleCall(const Program& program, size_t& i)
    {
        auto variable = getStringObjectValue(program, ++i);
        if (variable != nullptr)
        {
            executeFun(variable->getHash(), program.getLines()[i - 1], std::vector<Value>(), true);
        }
    }

    void VM::handlePrefix(const Program& program, size_t i)
    {
        auto& code = program.getCode()[i];
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
            _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid prefix operator!");
    }

    void VM::handleInfix(const Program& program, size_t i)
    {
        auto& code = program.getCode()[i];
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
                _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid infix operator for boolean!");
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
                _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid infix operator for number!");
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
                    _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid infix operator for string!");
                    break;
                }
            }
            else
                _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid infix operator for objects!");
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
                    std::make_tuple(program.getLines()[i],
                                    "VM: Invalid infix parameter! Its only possible to 'add' numbers and strings!");
                break;
            }
        }
        else
            _vmError = std::make_tuple(program.getLines()[i], "VM: Invalid infix parameter combination!");
    }

    void VM::handleIndex(const Program& program, size_t i, bool set)
    {
        auto indexValue = popValue();
        if (indexValue.getType() != ValueType::NUMBER)
            _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Index must be a number!");
        else
        {
            auto arrayValue = popValue();
            if (arrayValue.getType() != ValueType::OBJECT || arrayValue.object->getType() != ObjectType::ARRAY)
                _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Only arrays support indexing!");
            else
            {
                auto arrayObj = static_cast<ArrayObj*>(arrayValue.object);
                if (indexValue.number + 1 > arrayObj->getValues().size())
                    arrayObj->getValues().resize(indexValue.number + 1);

                if (set)
                    arrayObj->getValues()[indexValue.number] = popValue();
                else
                    _stack.push_back(arrayObj->getValues()[indexValue.number]);
            }
        }
    }

    void VM::executeFun(uint32_t hash, int line, std::vector<Value> parameters, bool paramFromStack)
    {
        auto funValue = _frame->get(hash);
        FunObj* funObj = nullptr;
        NativeFunObj* nativeFunObj = nullptr;

        if (funValue.getType() == ValueType::OBJECT && funValue.object->getType() == ObjectType::FUN)
            funObj = static_cast<FunObj*>(funValue.object);
        else if (funValue.getType() == ValueType::OBJECT && funValue.object->getType() == ObjectType::NATIVEFUN)
            nativeFunObj = static_cast<NativeFunObj*>(funValue.object);
        else
            _vmError = std::make_tuple(line, "VM: Variable name is not a function object!");

        if (funObj != nullptr)
        {
            _callFrame = std::make_shared<Frame>(_frame);
            for (size_t i = 0; i < funObj->getParameters().size(); i++)
            {
                if (paramFromStack)
                    _callFrame->addSet(funObj->getParameters()[i], popValue());
                else
                    _callFrame->addSet(funObj->getParameters()[i], parameters[i]);
            }
            run(funObj->getProgram());
        }
        else if (nativeFunObj != nullptr)
        {
            Frame nativeCallFrame(_frame);
            for (int i = 0; i < nativeFunObj->getArity(); i++)
            {
                char argName[10];
                sprintf(argName, "arg%d", i);

                if (paramFromStack)
                    nativeCallFrame.addSet(std::string(argName), popValue());
                else
                    nativeCallFrame.addSet(std::string(argName), parameters[i]);
            }
            auto value = nativeFunObj->getFun()(nativeCallFrame);
            if (value.getType() != ValueType::NIL)
                _stack.push_back(value);
        }
    }

    StringObj* VM::getStringObjectValue(const Program& program, int i)
    {
        auto& value = program.getConstant(program.getCode()[i]);
        if (value.getType() == ValueType::OBJECT && value.object->getType() == ObjectType::STRING)
            return static_cast<StringObj*>(value.object);
        else
        {
            _vmError = std::make_tuple(program.getLines()[i - 1], "VM: Variable name is not a string object!");
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
