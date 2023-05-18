#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "program/program.hpp"

#include <vector>
#include <cinttypes>

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
        std::string printClosures(const Program& program) const;

        uint16_t _depth = 0;
    };
}

#endif // DISASSEMBLER_H