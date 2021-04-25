#ifndef PROGRAM_H
#define PROGRAM_H

#include "value.hpp"
#include "objects/object.hpp"
#include "blinkscript/scanner/model/token.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class Program
    {
    public:
        void addCode(uint16_t code, uint16_t line);
        void removeCode(uint16_t count);

        void addValueCode(float number, uint16_t line);
        void addValueCode(bool boolean, uint16_t line);
        void addArrayValueCode(uint32_t arraySize, uint16_t line);
        void addStrValueCode(const std::string& string, uint16_t line);
        void addFunValueCode(Program&& program, const std::vector<Token>& funParameters, uint16_t line);

        const std::vector<uint16_t>& getCode() const;
        const std::vector<uint16_t>& getLines() const;
        const Value& getConstant(uint16_t index) const;

        int32_t searchConstant(Value searchConst) const;

        uint32_t getConstantSize() const;
        uint32_t getObjectSize() const;

    private:      
        uint32_t addValue(Value value, std::shared_ptr<Object> object = nullptr);

        std::vector<uint16_t> _code;
        std::vector<uint16_t> _lines;
        std::vector<Value> _constants;
        std::vector<std::shared_ptr<Object>> _objects;
    };
}

#endif // PROGRAM_H