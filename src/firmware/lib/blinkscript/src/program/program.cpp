#include "program.hpp"

#include "model/objects/string_object.hpp"
#include "model/op_codes.hpp"

namespace SyncBlink
{
    void Program::addCode(uint16_t code, size_t line)
    {
        _code.push_back(code);
        _lines.push_back(line);
    }

    void Program::removeCode(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            _code.pop_back();
            _lines.pop_back();
        }
    }

    uint16_t Program::addConstant(Value value, std::shared_ptr<Object> object)
    {
        bool found = false;
        uint16_t constantIndex;
        found = searchConstant(value, constantIndex);

        if (!found)
        {
            _constants.push_back(value);
            constantIndex = (uint16_t)_constants.size() - 1;

            if (object != nullptr)
            {
                object->index = (uint16_t)_objects.size();
                _objects.push_back(object);
            }
        }

        return constantIndex;
    }

    const std::vector<uint16_t>& Program::getCode() const
    {
        return _code;
    }

    const std::vector<size_t>& Program::getLines() const
    {
        return _lines;
    }

    const std::vector<Value>& Program::getConstants() const
    {
        return _constants;
    }

    const std::vector<std::shared_ptr<Object>>& Program::getObjects() const
    {
        return _objects;
    }

    bool Program::searchConstant(Value searchValue, uint16_t& foundIndex) const
    {
        bool found = false;
        for (uint16_t i = 0; i < (uint16_t)_constants.size(); i++)
        {
            auto constant = _constants[i];
            if (searchValue.getType() == ValueType::BOOL && constant.getType() == ValueType::BOOL &&
                searchValue.boolean == constant.boolean)
            {
                found = true;
                foundIndex = i;
                break;
            }
            else if (searchValue.getType() == ValueType::NUMBER && constant.getType() == ValueType::NUMBER &&
                     searchValue.number == constant.number)
            {
                found = true;
                foundIndex = i;
                break;
            }
            else if (searchValue.getType() == ValueType::OBJECT && constant.getType() == ValueType::OBJECT &&
                     searchValue.object->getType() == ObjectType::STRING && constant.object->getType() == ObjectType::STRING)
            {
                auto searchStr = searchValue.getObject<StringObj>()->getString();
                auto constStr = constant.getObject<StringObj>()->getString();
                if (searchStr == constStr)
                {
                    found = true;
                    foundIndex = i;
                    break;
                }
            }
        }
        return found;
    }
}