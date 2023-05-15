#include "function_object.hpp"

namespace SyncBlink
{
    FunObj::FunObj(Program&& program) : _program(std::move(program))
    {
    }

    void FunObj::addParameter(const std::string& parameter)
    {
        _parameters.push_back(parameter);
    }

    
    void FunObj::setForeignLocalIndices(const std::vector<uint16_t>& foreignLocalIndices)
    {
        _foreignLocalIndices = foreignLocalIndices;
    }

    ObjectType FunObj::getType() const
    {
        return ObjectType::FUN;
    }

    const Program& FunObj::getProgram() const
    {
        return _program;
    }

    const std::vector<std::string>& FunObj::getParameters() const
    {
        return _parameters;
    }
        
    const std::vector<uint16_t>& FunObj::getForeignLocalIndices() const
    {
        return _foreignLocalIndices;
    }
}