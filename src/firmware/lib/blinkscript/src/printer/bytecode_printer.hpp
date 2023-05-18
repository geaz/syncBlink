#ifndef BYTECODEPRINTER_H
#define BYTECODEPRINTER_H

#include "program/program.hpp"

namespace SyncBlink
{
    class ByteCodePrinter
    {
    public:
        std::vector<uint8_t> print(const Program& program) const;

    private:
        void addSizeAndCount(std::vector<uint8_t>& byteCode, const Program& program) const;
        void addCodeAndLines(std::vector<uint8_t>& byteCode, const Program& program) const;
        void addObjects(std::vector<uint8_t>& byteCode, const Program& program) const;
        void addConstants(std::vector<uint8_t>& byteCode, const Program& program) const;

        void addValue(const Value& value, std::vector<uint8_t>& byteCode) const;
        void writeTwoByteValue(uint16_t value, std::vector<uint8_t>& byteCode) const;
        void writeFourByteValue(uint32_t value, std::vector<uint8_t>& byteCode) const;
        void writeString(const std::string& value, std::vector<uint8_t>& byteCode, const Program& program) const;
    };
}

#endif // BYTECODEPRINTER_H