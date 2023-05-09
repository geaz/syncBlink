#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "program/model/value.hpp"

#include <vector>

namespace SyncBlink
{
    static const float TableLoad = 0.75;

    struct TableEntry
    {
        uint32_t hash;
        Value value;
    };

    class SymbolTable
    {
    public:
        SymbolTable();

        bool set(uint32_t hash, Value value);
        bool get(uint32_t hash, Value* value);

    private:
        TableEntry* find(std::vector<TableEntry>& entries, int capacity, uint32_t hash);
        void adjustCapacity();

        uint32_t _count = 0;
        uint32_t _capacity = 8;
        std::vector<TableEntry> _entries;
    };
}

#endif // SYMBOLTABLE_H