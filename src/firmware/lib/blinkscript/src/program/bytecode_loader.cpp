#include "bytecode_loader.hpp"

#include "model/objects/array_object.hpp"
#include "model/objects/function_object.hpp"
#include "model/objects/string_object.hpp"
#include "program/model/value.hpp"
#include "source/bytecode_source.hpp"

#include <cstring>
#include <memory>

namespace SyncBlink
{
    ByteCodeLoader::ByteCodeLoader(std::shared_ptr<ByteCodeSource> byteCode) : _byteCode(byteCode)
    {
    }

    Program ByteCodeLoader::getProgram(size_t startIdx)
    {
        Program program;

        size_t idx = startIdx;
        MAXCODE codeSize = loadTwoBytes(idx);
        MAXITEM objCount = _byteCode->getByte(idx++);
        MAXITEM constantCount = _byteCode->getByte(idx++);

        loadCode(idx, program, codeSize);
        storeObjects(idx, program, objCount);
        loadConstants(idx, program, constantCount);

        return std::move(program);
    }

    void ByteCodeLoader::loadCode(size_t& idx, Program& program, const MAXCODE& codeSize)
    {
        size_t curIdx = idx - 1;
        while (idx < codeSize * 3 /* (code 1B & line 2B) = 3 Byte */ + curIdx - 1)
        {
            CODETYPE code = _byteCode->getByte(idx++);
            LINETYPE line = loadTwoBytes(idx);
            program.addCode(code, line);
        }
    }

    void ByteCodeLoader::storeObjects(size_t& idx, Program& program, const MAXITEM& objCount)
    {
        for (MAXITEM i = 0; i < objCount; i++)
        {
            auto type = (ObjectType)_byteCode->getByte(idx++);
            size_t size = loadTwoBytes(idx); 

            switch (type)
            {
            case ObjectType::STRING: {
                std::vector<uint8_t> strBytes;
                for (MAXCODE j = 0; j < size; j++)
                    strBytes.push_back(_byteCode->getByte(idx++));

                _objects.push_back(std::make_shared<StringObj>(std::string(strBytes.begin(), strBytes.end())));
                break;
            }
            case ObjectType::FUN: {
                ByteCodeLoader funLoad(_byteCode);
                Program funProg = funLoad.getProgram(idx);
                idx += size;

                auto funObj = std::make_shared<FunObj>(std::move(funProg));
                auto paramCount = loadTwoBytes(idx);
                for (size_t j = 0; j < paramCount; j++)
                {
                    std::vector<uint8_t> strBytes;
                    uint16_t strSize = loadTwoBytes(idx);
                    for (uint16_t j = 0; j < strSize; j++)
                        strBytes.push_back(_byteCode->getByte(idx++));

                    funObj->addParameter(std::string(strBytes.begin(), strBytes.end()));
                }

                _objects.push_back(funObj);
                break;
            }
            case ObjectType::ARRAY: {
                MAXITEM objIndex = 0;
                std::vector<Value> values;
                for (MAXITEM j = 0; j < size; j++)
                    values.push_back(loadValue(idx, program, objIndex));

                _objects.push_back(std::make_shared<ArrayObj>(values));
                break;
            }
            case ObjectType::NATIVEFUN:
                break;
            }
        }
    }

    void ByteCodeLoader::loadConstants(size_t& idx, Program& program, const MAXITEM& constantCount)
    {
        MAXITEM objIndex = 0;
        for (MAXITEM i = 0; i < constantCount; i++)
        {
            Value value = loadValue(idx, program, objIndex);
            if (value.getType() == ValueType::OBJECT) program.addConstant(value, _objects[objIndex]);
            else program.addConstant(value, nullptr);
        }
    }

    Value ByteCodeLoader::loadValue(size_t& idx, Program& program, MAXITEM& objIndex)
    {
        Value value;
        auto type = (ValueType)_byteCode->getByte(idx++);
        switch (type)
        {
        case ValueType::BOOL:
            value = Value((bool)_byteCode->getByte(idx++));
            break;
        case ValueType::NUMBER: {
            float number;
            uint32_t floatBytes = loadFourBytes(idx);
            memcpy(&number, &floatBytes, sizeof(float));

            value = Value(number);
            break;
        }
        case ValueType::OBJECT:
        {
            objIndex = _byteCode->getByte(idx++);
            auto objPtr = _objects[objIndex];
            value = Value(objPtr.get());
            break;
        }
        case ValueType::NIL:
            break;
        }
        return value;
    }

    uint16_t ByteCodeLoader::loadTwoBytes(size_t& idx)
    {
        // Prevent -Wunsequenced warnings
        size_t firstByte = idx++;
        return _byteCode->getByte(firstByte) | _byteCode->getByte(idx++) << 8;
    }

    uint32_t ByteCodeLoader::loadFourBytes(size_t& idx)
    {
        // Prevent -Wunsequenced warnings
        size_t firstByte = idx++;
        size_t secondByte = idx++;
        size_t thirdByte = idx++;
        return _byteCode->getByte(firstByte) | _byteCode->getByte(secondByte) << 8 | _byteCode->getByte(thirdByte) << 16 |
               _byteCode->getByte(idx++) << 24;
    }
}