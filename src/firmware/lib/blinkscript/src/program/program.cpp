#include "program.hpp"

#include "model/objects/string_object.hpp"
#include "model/op_codes.hpp"

#include <algorithm>

namespace SyncBlink
{
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

    uint32_t Program::addValue(Value value, std::shared_ptr<Object> object)
    {
        int32_t constantIndex = searchConstant(value);
        if (constantIndex == -1)
        {
            _constants.push_back(value);
            constantIndex = _constants.size() - 1;

            if (object != nullptr)
            {
                object->index = _objects.size();
                _objects.push_back(object);
            }
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

    const Object* Program::getObject(uint16_t index) const
    {
        return _objects[index].get();
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
                     searchConst.object->getType() == ObjectType::STRING && constant.object->getType() == ObjectType::STRING)
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