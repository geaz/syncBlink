#include "disassembler.hpp"

#include "program/model/objects/array_object.hpp"
#include "program/model/objects/object.hpp"
#include "program/model/objects/string_object.hpp"
#include "program/model/objects/closure_object.hpp"
#include "program/model/op_codes.hpp"

#include <iomanip>
#include <sstream>

namespace SyncBlink
{
    std::string Disassembler::print(const Program& program) const
    {
        std::string disCode = printObjectCount(program);
        disCode += printConstants(program);
        disCode += printCode(program);
        disCode += printClosures(program);
        return disCode;
    }

    std::string Disassembler::printObjectCount(const Program& program) const
    {
        std::stringstream ss;
        ss << ".object.count " << program.getObjects().size() << "\n\n";

        return ss.str();
    }

    std::string Disassembler::printConstants(const Program& program) const
    {
        std::string disCode = ".constants:\n";

        std::stringstream ss;
        for (uint16_t i = 0; i < program.getConstants().size(); i++)
        {
            ss << "\t@" << std::setw(3) << i << ": " << printConstant(program.getConstants()[i]) << "\n";
            disCode += ss.str();
            ss.str("");
        }

        return disCode + "\n";
    }

    std::string Disassembler::printConstant(Value constant) const
    {
        std::stringstream ss;
        switch (constant.getType())
        {
        case ValueType::BOOL:
            ss << (constant.boolean ? "TRUE" : "FALSE");
            break;
        case ValueType::NUMBER:
            ss << constant.number;
            break;
        case ValueType::OBJECT: {
            switch (constant.object->getType())
            {
            case ObjectType::CLOSURE:
                ss << "CLOSURE";
                break;
            case ObjectType::FUN:
                ss << "FUN";
                break;
            case ObjectType::NATIVEFUN:
                ss << "NATIVEFUN";
                break;
            case ObjectType::STRING: {
                auto strObj = constant.getObject<StringObj>();
                ss << "\"" << strObj->getString() << "\"";
                break;
            }
            case ObjectType::ARRAY: {
                ss << "[";
                auto arrayObj = constant.getObject<ArrayObj>();
                auto& items = arrayObj->getValues();
                for (uint16_t i = 0; i < items.size(); i++)
                {
                    ss << printConstant(items[i]);
                    if (i < items.size() - 1) ss << ", ";
                }
                ss << "]";
                break;
            }
            }
            break;
        }
        case ValueType::NIL:
            break;
        }
        return ss.str();
    }

    std::string Disassembler::printCode(const Program& program) const
    {
        std::string disCode = ".code:\n";

        std::stringstream ss;
        for (uint16_t i = 0; i < program.getCode().size(); i++)
        {
            ss << "\t" << std::setw(3) << i << ": ";

            switch (program.getCode()[i])
            {
            case OpCode::VALUE:
                ss << "VALUE\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::CLOSURE:
                ss << "CLOSURE\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::DEFINE_GLOBAL:
                ss << "DEFINE_GLOBAL\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::LOAD_GLOBAL:
                ss << "LOAD_GLOBAL\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::LOAD_LOCAL:
                ss << "LOAD_LOCAL\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::SET_GLOBAL:
                ss << "SET_GLOBAL\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::SET_LOCAL:
                ss << "SET_LOCAL\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            case OpCode::SET_INDEX:
                ss << "SET_INDEX\n";
                break;
            case OpCode::CALL:
                ss << "CALL\n";
                break;
            case OpCode::BANG_NEGATE:
                ss << "BANG_NEGATE\n";
                break;
            case OpCode::NEGATE:
                ss << "NEGATE\n";
                break;
            case OpCode::ADD:
                ss << "ADD\n";
                break;
            case OpCode::SUB:
                ss << "SUB\n";
                break;
            case OpCode::DIV:
                ss << "DIV\n";
                break;
            case OpCode::MUL:
                ss << "MUL\n";
                break;
            case OpCode::MODULO:
                ss << "MODULO\n";
                break;
            case OpCode::AND:
                ss << "AND\n";
                break;
            case OpCode::OR:
                ss << "OR\n";
                break;
            case OpCode::LESS:
                ss << "LESS\n";
                break;
            case OpCode::LESS_EQUAL:
                ss << "LESS_EQUAL\n";
                break;
            case OpCode::GREATER:
                ss << "GREATER\n";
                break;
            case OpCode::GREATER_EQUAL:
                ss << "GREATER_EQUAL\n";
                break;
            case OpCode::EQUAL_EQUAL:
                ss << "EQUAL_EQUAL\n";
                break;
            case OpCode::BANG_EQUAL:
                ss << "BANG_EQUAL\n";
                break;
            case OpCode::JMP:
                ss << "JMP\n";
                break;
            case OpCode::JMP_NOT:
                ss << "JMP_NOT\n";
                break;
            case OpCode::INDEX:
                ss << "INDEX\n";
                break;
            case OpCode::POP:
                ss << "POP\n";
                break;
            case OpCode::CLEAR_SCOPE:
                ss << "CLEAR_SCOPE\n";
                ss << "\t" << std::setw(3) << ++i << ": ." << program.getCode()[i] << "\n";
                break;
            }
        }

        return disCode + ss.str();
    }

    std::string Disassembler::printClosures(const Program& program) const
    {
        std::string disCode = "\n.closures:\n\n";
        std::stringstream ss;
        uint16_t closureCount = 0;
        for (uint16_t i = 0; i < program.getConstants().size(); i++)
        {
            Value constant = program.getConstants()[i];
            if (constant.getType() == ValueType::OBJECT && constant.object->getType() == ObjectType::FUN)
            {
                auto funObj = constant.getObject<FunObj>();

                ss << "\t." << i << ":\n";
                ss << "\t.foreign.count: " << funObj->getForeignLocalIndices().size() << "\n";
                ss << "{";
                ss << print(funObj->getProgram());
                ss << "}";
                disCode += ss.str();

                ss.str("");
                closureCount++;
            }
        }
        return disCode;
    }
}