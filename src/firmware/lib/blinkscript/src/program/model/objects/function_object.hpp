#ifndef FUNOBJ_H
#define FUNOBJ_H

#include "object.hpp"
#include "program/program.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    class FunObj : public Object
    {
    public:
        FunObj(Program&& program);

        void addParameter(const std::string& parameter);

        ObjectType getType() const;
        const Program& getProgram() const;
        const std::vector<std::string>& getParameters() const;

    private:
        Program _program;
        std::vector<std::string> _parameters;
    };
}

#endif // FUNOBJ_H