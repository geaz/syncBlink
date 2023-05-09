#ifndef BYTECODE_LODER_H
#define BYTECODE_LODER_H

#include "program/program.hpp"
#include "source/bytecode_source.hpp"
#include "source/script_source.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class ByteCodeLoader
    {
    public:
        ByteCodeLoader(std::shared_ptr<ByteCodeSource> byteCode);

        Program getProgram(size_t startIdx = 0);

    private:
        void loadCode(size_t& idx, Program& program, const size_t& codeSize);
        void storeObjects(size_t& idx, Program& program, const uint16_t& objCount);
        void loadConstants(size_t& idx, Program& program, const uint16_t& constantCount);
        Value loadValue(size_t& idx, Program& program, uint16_t& objIndex);

        uint16_t loadTwoBytes(size_t& idx);
        uint32_t loadFourBytes(size_t& idx);

        std::shared_ptr<ByteCodeSource> _byteCode;
        std::vector<std::shared_ptr<Object>> _objects;
    };
}

#endif // BYTECODE_LODER_H