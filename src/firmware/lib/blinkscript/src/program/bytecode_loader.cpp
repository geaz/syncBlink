#include "bytecode_loader.hpp"

#include "source/bytecode_source.hpp"
#include "program/model/value.hpp"
#include "model/objects/string_object.hpp"
#include "model/objects/function_object.hpp"
#include "model/objects/array_object.hpp"

#include <memory>

namespace SyncBlink
{
    ByteCodeLoader::ByteCodeLoader(std::shared_ptr<ByteCodeSource> byteCode, std::shared_ptr<ScriptSource> source)
        : _byteCode(byteCode), _source(source) { }

    Program ByteCodeLoader::getProgram(size_t startIdx)
    {
        Program program;

        size_t idx = startIdx;
        size_t codeSize = loadFourBytes(idx);
        uint16_t objCount = loadTwoBytes(idx);
        uint16_t constantCount = loadTwoBytes(idx);

        loadCode(idx, program, codeSize);
        storeObjects(idx, program, objCount);
        loadConstants(idx, program, constantCount);

        return std::move(program);
    }

    void ByteCodeLoader::loadCode(size_t& idx, Program& program, const size_t& codeSize)
    {
        size_t curIdx = idx - 1;
        while(idx < codeSize * 4 /* 2 x uint16_t (code & line) = 4 Byte */ + curIdx - 1)
        {
            uint16_t code = loadTwoBytes(idx);
            uint16_t line = loadTwoBytes(idx);
            program.addCode(code, line);
        }
    }

    void ByteCodeLoader::storeObjects(size_t& idx, Program& program, const uint16_t& objCount)
    {
        for(uint16_t i = 0; i < objCount; i++)
        {
            auto type = (ObjectType)_byteCode->getByte(idx++);
            size_t size = loadFourBytes(idx);

            switch(type)
            {
                case ObjectType::STRING:
                {
                    std::vector<uint8_t> strBytes;
                    for(size_t j = 0; j < size; j++)
                        strBytes.push_back(_byteCode->getByte(idx++));

                    _objects.push_back(std::make_shared<StringObj>(std::string(strBytes.begin(), strBytes.end())));
                    break;
                }
                case ObjectType::FUN:
                {
                    ByteCodeLoader funLoad(_byteCode, _source);
                    Program funProg = funLoad.getProgram(idx);
                    idx += size;

                    auto funObj = std::make_shared<FunObj>(std::move(funProg));
                            
                    auto paramCount = loadFourBytes(idx);
                    for(size_t j = 0; j < paramCount; j++)
                    {
                        std::vector<uint8_t> strBytes;
                        size_t strSize = loadFourBytes(idx);
                        for(size_t j = 0; j < strSize; j++)
                            strBytes.push_back(_byteCode->getByte(idx++));

                        funObj->addParameter(std::string(strBytes.begin(), strBytes.end()));
                    }

                    _objects.push_back(funObj);
                    break;
                }
                case ObjectType::ARRAY:
                {
                    uint16_t objIndex = 0;
                    std::vector<Value> values;
                    for(size_t j = 0; j < size; j++)
                        values.push_back(loadValue(idx, program, objIndex));
                    
                    _objects.push_back(std::make_shared<ArrayObj>(values));
                    break;
                }
            }
        }
    }

    void ByteCodeLoader::loadConstants(size_t& idx, Program& program, const uint16_t& constantCount)
    {
        uint16_t objIndex = 0;
        for(uint16_t i = 0; i < constantCount; i++)
        {
            Value value = loadValue(idx, program, objIndex);
            if(value.getType() == ValueType::OBJECT)
                program.addValue(value, _objects[objIndex]);
            else
                program.addValue(value, nullptr);
        }
    }

    Value ByteCodeLoader::loadValue(size_t& idx, Program& program, uint16_t& objIndex)
    {
        Value value;
        auto type = (ValueType)_byteCode->getByte(idx++);
        switch(type)
        {
            case ValueType::BOOL:
                value = Value((bool)_byteCode->getByte(idx++));
                break;
            case ValueType::NUMBER:
                value = Value((float)loadFourBytes(idx));
                break;
            case ValueType::OBJECT:
                objIndex = loadTwoBytes(idx);
                auto objPtr = _objects[objIndex];
                value = Value(objPtr.get());
                break;
        }
        return value;
    }

    uint16_t ByteCodeLoader::loadTwoBytes(size_t& idx)
    {
        // Prevent -Wunsequenced warnings
        size_t firstByte = idx++;
        return    _byteCode->getByte(firstByte)
                | _byteCode->getByte(idx++) << 8;
    }

    uint32_t ByteCodeLoader::loadFourBytes(size_t& idx)
    {
        // Prevent -Wunsequenced warnings
        size_t firstByte = idx++;
        size_t secondByte = idx++;
        size_t thirdByte = idx++;
        return    _byteCode->getByte(firstByte)
                | _byteCode->getByte(secondByte) << 8                            
                | _byteCode->getByte(thirdByte) << 16
                | _byteCode->getByte(idx++) << 24;
    }
}