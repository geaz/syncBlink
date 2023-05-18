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
        writeTwoByteValue((MAXCODE)program.getCode().size(), byteCode);
        byteCode.push_back((MAXITEM)program.getObjects().size() & 0xFF);
        byteCode.push_back((MAXITEM)program.getConstants().size() & 0xFF);
    }

    void ByteCodePrinter::addCodeAndLines(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        for (size_t i = 0; i < program.getCode().size(); i++)
        {
            byteCode.push_back(program.getCode()[i] & 0xFF);
            writeTwoByteValue(program.getLines()[i], byteCode);
        }
    }

    void ByteCodePrinter::addObjects(std::vector<uint8_t>& byteCode, const Program& program) const
    {
        for (MAXITEM i = 0; i < (MAXITEM)program.getObjects().size(); i++)
        {
            auto obj = program.getObjects()[i];
            byteCode.push_back((uint8_t)obj->getType());
            switch (obj->getType())
            {
            case ObjectType::STRING: {
                auto strObj = static_cast<const StringObj*>(obj.get());
                writeString(strObj->getString(), byteCode, program);
                break;
            }
            case ObjectType::FUN: {
                auto funObj = static_cast<const FunObj*>(obj.get());
                auto funProg = print(funObj->getProgram());

                writeTwoByteValue(funProg.size(), byteCode);
                for (size_t j = 0; j < funProg.size(); j++)
                    byteCode.push_back(funProg[j]);

                writeTwoByteValue(funObj->getParameters().size(), byteCode);
                for (size_t j = 0; j < funObj->getParameters().size(); j++)
                    writeString(funObj->getParameters()[j], byteCode, program);

                break;
            }
            case ObjectType::ARRAY: {
                auto arrayObj = static_cast<const ArrayObj*>(obj.get());
                writeTwoByteValue(arrayObj->getValuesConst().size(), byteCode);
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
        for (MAXITEM i = 0; i < (MAXITEM)program.getConstants().size(); i++)
        {
            const Value value = program.getConstants()[i];
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
            MAXITEM objIndex = objPointer->index;
            byteCode.push_back(objIndex & 0xFF);
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
        writeTwoByteValue(strSize, byteCode);
        for (size_t i = 0; i < strSize; i++)
            byteCode.push_back(value[i]);
    }
}