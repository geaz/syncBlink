#ifndef PROGRAM_H
#define PROGRAM_H

#include "model/objects/object.hpp"
#include "model/value.hpp"
#include "parser/model/token.hpp"
#include "source/script_source.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class Program
    {
    public:
        void addCode(uint16_t code, size_t line);
        void removeCode(size_t count);

        uint16_t addConstant(Value value, std::shared_ptr<Object> object = nullptr);

        const std::vector<uint16_t>& getCode() const;
        const std::vector<size_t>& getLines() const;
        const std::vector<Value>& getConstants() const;
        const std::vector<std::shared_ptr<Object>>& getObjects() const;

    private:
        bool searchConstant(Value searchValue, uint16_t& foundIndex) const;

        std::vector<uint16_t> _code;
        std::vector<size_t> _lines;
        std::vector<Value> _constants;
        std::vector<std::shared_ptr<Object>> _objects;
    };
}

#endif // PROGRAM_H