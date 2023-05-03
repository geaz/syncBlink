#include "program.hpp"

#include "vm/op_codes.hpp"
#include "objects/array_object.hpp"
#include "objects/function_object.hpp"
#include "objects/string_object.hpp"

#include <algorithm>

namespace SyncBlink
{
    Program::Program(std::shared_ptr<ScriptSource> source) : _source(source) { }

    void Program::addCode(uint16_t code, uint16_t line)
    {
        _code.push_back(code);
        _lines.push_back(line);
    }

    void Program::removeCode(uint16_t count)
    {
        for (uint16_t i = 0; i < count; i++)
        {
            _code.pop_back();
            _lines.pop_back();
        }
    }

    void Program::addValueCode(float number, uint16_t line)
    {
        addCode(addValue(Value(number)), line);
    }

    void Program::addValueCode(bool boolean, uint16_t line)
    {
        addCode(addValue(Value(boolean)), line);
    }

    void Program::addArrayValueCode(uint32_t arraySize, uint16_t line)
    {
        std::vector<Value> arrayContent;
        for (uint32_t i = 1; i <= arraySize; i++)
        {
            // Add the last 'arraySize' count values to the array content
            arrayContent.push_back(_constants[getConstantSize() - i]);
        }
        std::reverse(arrayContent.begin(), arrayContent.end());

        auto ptr = std::make_shared<ArrayObj>(arrayContent);
        addCode(addValue(Value(ptr.get()), ptr), line);
    }

    void Program::addStrValueCode(const std::string& string, uint16_t line)
    {
        auto ptr = std::make_shared<StringObj>(StringObj(string));
        addCode(addValue(Value(ptr.get()), ptr), line);
    }

    void Program::addFunValueCode(Program&& program, const std::vector<Token>& funParameters, uint16_t line)
    {
        auto ptr = std::make_shared<FunObj>(FunObj(std::move(program)));
        for (const Token& token : funParameters)
        {
            ptr->addParameter(token.getLexem(_source));
        }
        addCode(addValue(Value(ptr.get()), ptr), line);
    }

    uint32_t Program::addValue(Value value, std::shared_ptr<Object> object)
    {
        int32_t constantIndex = searchConstant(value);
        if (constantIndex == -1)
        {
            _constants.push_back(value);
            constantIndex = _constants.size() - 1;

            if (object != nullptr)
                _objects.push_back(std::move(object));
        }
        return constantIndex;
    }

    const std::vector<uint16_t>& Program::getCode() const
    {
        return _code;
    }

    const std::vector<uint16_t>& Program::getLines() const
    {
        return _lines;
    }
    
    const Value& Program::getConstant(uint16_t index) const
    {
        return _constants[index];
    }

    int32_t Program::searchConstant(Value searchConst) const
    {
        int32_t foundIndex = -1;
        for (size_t i = 0; i < _constants.size(); i++)
        {
            auto constant = _constants[i];
            if (searchConst.getType() == ValueType::BOOL && constant.getType() == ValueType::BOOL &&
                searchConst.boolean == constant.boolean)
            {
                foundIndex = i;
                break;
            }
            else if (searchConst.getType() == ValueType::NUMBER && constant.getType() == ValueType::NUMBER &&
                     searchConst.number == constant.number)
            {
                foundIndex = i;
                break;
            }
            else if (searchConst.getType() == ValueType::OBJECT && constant.getType() == ValueType::OBJECT &&
                     searchConst.object->getType() == ObjectType::STRING &&
                     constant.object->getType() == ObjectType::STRING)
            {
                auto searchStr = searchConst.getObject<StringObj>()->getString();
                auto constStr = constant.getObject<StringObj>()->getString();
                if (searchStr == constStr)
                {
                    foundIndex = i;
                    break;
                }
            }
        }
        return foundIndex;
    }

    uint32_t Program::getObjectSize() const
    {
        return _objects.size();
    }
    uint32_t Program::getConstantSize() const
    {
        return _constants.size();
    }
}