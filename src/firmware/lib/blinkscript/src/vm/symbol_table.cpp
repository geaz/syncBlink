#include "symbol_table.hpp"

#include "program/model/string_hash.hpp"

namespace SyncBlink
{
    SymbolTable::SymbolTable()
    {
        _entries.resize(_capacity);
    }

    bool SymbolTable::set(const std::string& identifier, Value value)
    {
        return set(stringHash(identifier), value);
    }

    bool SymbolTable::set(uint32_t hash, Value value)
    {
        if (_count + 1 > _capacity * SyncBlink::TableLoad) adjustCapacity();

        TableEntry* entry = find(_entries, _capacity, hash);
        bool isNewKey = entry->value.getType() == ValueType::NIL;
        if (isNewKey) _count++;

        entry->hash = hash;
        entry->value = value;

        return isNewKey;
    }

    bool SymbolTable::get(const std::string& identifier, Value* value)
    {
        return get(stringHash(identifier), value);
    }

    bool SymbolTable::get(uint32_t hash, Value* value)
    {
        if (_count == 0) return false;

        TableEntry* entry = find(_entries, _capacity, hash);
        if (entry->value.getType() == ValueType::NIL) return false;

        *value = entry->value;
        return true;
    }

    bool SymbolTable::hasRefOnObject(const Object* obj)
    {
        bool hasRef = false;
        for (auto iter = _entries.begin(); iter != _entries.end(); iter++)
        {
            if (iter->value.getType() == ValueType::OBJECT && iter->value.object == obj)
            {
                hasRef = true;
                break;
            }
        }
        return hasRef;
    }

    TableEntry* SymbolTable::find(std::vector<TableEntry>& entries, int capacity, uint32_t hash)
    {
        uint32_t index = hash & (capacity - 1);
        for (;;)
        {
            TableEntry* entry = &entries[index];
            if (entry->value.getType() == ValueType::NIL) return entry;
            else if (entry->hash == hash)
                return entry;
            index = (index + 1) & (capacity - 1);
        }
    }

    void SymbolTable::adjustCapacity()
    {
        uint32_t newCapacity = _capacity * 2;

        std::vector<TableEntry> newEntries;
        newEntries.resize(newCapacity);

        _count = 0;
        for (uint32_t i = 0; i < _capacity; i++)
        {
            TableEntry* entry = &_entries[i];
            if (entry->value.getType() == ValueType::NIL) continue;

            TableEntry* dest = find(newEntries, newCapacity, entry->hash);
            dest->hash = entry->hash;
            dest->value = entry->value;
            _count++;
        }

        _entries = newEntries;
        _capacity = newCapacity;
    }
}