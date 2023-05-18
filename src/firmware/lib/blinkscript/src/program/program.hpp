#ifndef PROGRAM_H
#define PROGRAM_H

#include "model/objects/object.hpp"
#include "model/value.hpp"
#include "parser/model/token.hpp"
#include "source/script_source.hpp"
#include "program_types.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class Program
    {
    public:
        void addCode(CODETYPE code, LINETYPE line);
        void removeCode(MAXCODE count);

        MAXITEM addConstant(Value value, std::shared_ptr<Object> object = nullptr);

        const std::vector<CODETYPE>& getCode() const;
        const std::vector<LINETYPE>& getLines() const;
        const std::vector<Value>& getConstants() const;
        const std::vector<std::shared_ptr<Object>>& getObjects() const;

    private:
        bool searchConstant(Value searchValue, MAXITEM& foundIndex) const;

        std::vector<CODETYPE> _code;
        std::vector<LINETYPE> _lines;
        std::vector<Value> _constants;
        std::vector<std::shared_ptr<Object>> _objects;
    };
}

#endif // PROGRAM_H