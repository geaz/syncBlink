#include "bytecode_printer.hpp"

#include "program/model/objects/array_object.hpp"
#include "program/model/objects/function_object.hpp"
#include "program/model/objects/object.hpp"
#include "program/model/objects/string_object.hpp"
#include "program/model/op_codes.hpp"
#include "program/model/value.hpp"

#include <cstring>

namespace SyncBlink
{
    std::vector<uint8_t> ByteCodePrinter::print(const Program& program) const
    {
        std::vector<uint8_t> byteCode;
        addSizeAndCount(byteCode, program);
        addCodeAndLines(byteCode, program);
        addObjects(byteCode, program);
        addConstants(byteCode, program);
        return byteCode;
    }

    void ByteCodePrinter::addSizeAndCount(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        writeFourByteValue(program.getCode().size(), byteCode);
        writeTwoByteValue(program.getObjectSize(), byteCode);
        writeTwoByteValue(program.getConstantSize(), byteCode);
    }

    void ByteCodePrinter::addCodeAndLines(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        for (size_t i = 0; i < program.getCode().size(); i++)
        {
            writeTwoByteValue(program.getCode()[i], byteCode);
            writeTwoByteValue(program.getLines()[i], byteCode);
        }
    }

    void ByteCodePrinter::addObjects(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        for (size_t i = 0; i < program.getObjectSize(); i++)
        {
            const Object* obj = program.getObject(i);
            byteCode.push_back((uint8_t)obj->getType());
            switch (obj->getType())
            {
            case ObjectType::STRING: {
                auto strObj = static_cast<const StringObj*>(obj);
                writeString(strObj->getString(), byteCode, program);
                break;
            }
            case ObjectType::FUN: {
                auto funObj = static_cast<const FunObj*>(obj);
                auto funProg = print(funObj->getProgram());

                writeFourByteValue(funProg.size(), byteCode);
                for (size_t j = 0; j < funProg.size(); j++)
                    byteCode.push_back(funProg[j]);

                writeFourByteValue(funObj->getParameters().size(), byteCode);
                for (size_t j = 0; j < funObj->getParameters().size(); j++)
                    writeString(funObj->getParameters()[j], byteCode, program);

                break;
            }
            case ObjectType::ARRAY: {
                auto arrayObj = static_cast<const ArrayObj*>(obj);
                writeFourByteValue(arrayObj->getValuesConst().size(), byteCode);
                for (size_t j = 0; j < arrayObj->getValuesConst().size(); j++)
                    addValue(arrayObj->getValuesConst()[j], byteCode);
                break;
            }
            case ObjectType::NATIVEFUN:
                // We dont have to print nativefuns, because the will NEVER be present in a compiled blinkScript
                // NativeFuns get added during runtime only.
                break;
            }
        }
    }

    void ByteCodePrinter::addConstants(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        for (size_t i = 0; i < program.getConstantSize(); i++)
        {
            const Value value = program.getConstant(i);
            addValue(value, byteCode);
        }
    }

    void ByteCodePrinter::addValue(const Value& value, std::vector<uint8_t>& byteCode) const
    {
        byteCode.push_back((uint8_t)value.getType());
        switch (value.getType())
        {
        case ValueType::BOOL:
            byteCode.push_back(value.boolean);
            break;
        case ValueType::NUMBER: {
            uint32_t floatBytes;
            memcpy(&floatBytes, &value.number, sizeof(float));

            byteCode.push_back(floatBytes & 0xFF);
            byteCode.push_back((floatBytes >> 8) & 0xFF);
            byteCode.push_back((floatBytes >> 16) & 0xFF);
            byteCode.push_back((floatBytes >> 24) & 0xFF);
            break;
        }
        case ValueType::OBJECT: {
            Object* objPointer = value.object;
            uint16_t objIndex = objPointer->index;

            byteCode.push_back(objIndex & 0xFF);
            byteCode.push_back((objIndex >> 8) & 0xFF);
            break;
        }
        case ValueType::NIL:
            break;
        }
    }

    void ByteCodePrinter::writeTwoByteValue(uint16_t value, std::vector<uint8_t>& byteCode) const
    {
        byteCode.push_back(value & 0xFF);
        byteCode.push_back((value >> 8) & 0xFF);
    }

    void ByteCodePrinter::writeFourByteValue(uint32_t value, std::vector<uint8_t>& byteCode) const
    {
        byteCode.push_back(value & 0xFF);
        byteCode.push_back((value >> 8) & 0xFF);
        byteCode.push_back((value >> 16) & 0xFF);
        byteCode.push_back((value >> 24) & 0xFF);
    }

    void ByteCodePrinter::writeString(const std::string& value, std::vector<uint8_t>& byteCode, const Program& program) const
    {
        auto strSize = value.size();
        writeFourByteValue(strSize, byteCode);
        for (size_t i = 0; i < strSize; i++)
            byteCode.push_back(value[i]);
    }
}