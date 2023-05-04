#ifndef PROGRAM_H
#define PROGRAM_H

#include "model/value.hpp"
#include "model/objects/object.hpp"
#include "parser/model/token.hpp"
#include "source/script_source.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class Program
    {
    public:
        void addCode(uint16_t code, uint16_t line);
        void removeCode(uint16_t count);

        uint32_t addValue(Value value, std::shared_ptr<Object> object = nullptr);

        const std::vector<uint16_t>& getCode() const;
        const std::vector<uint16_t>& getLines() const;
        const Value& getConstant(uint16_t index) const;        
        const Object* getObject(uint16_t index) const;

        uint32_t getConstantSize() const;
        uint32_t getObjectSize() const;

    private:
        int32_t searchConstant(Value searchConst) const;

        std::vector<uint16_t> _code;
        std::vector<uint16_t> _lines;
        std::vector<Value> _constants;
        std::vector<std::shared_ptr<Object>> _objects;
    };
}

#endif // PROGRAM_H