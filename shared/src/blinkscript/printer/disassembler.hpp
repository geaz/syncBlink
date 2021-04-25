#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "blinkscript/vm/model/program.hpp"

namespace SyncBlink
{
    class Disassembler
    {
    public:
        std::string print(const Program& program) const;

    private:
        std::string printObjectCount(const Program& program) const;
        std::string printConstants(const Program& program) const;
        std::string printConstant(Value constant) const;        
        std::string printCode(const Program& program) const;
    };
}

#endif // DISASSEMBLER_H