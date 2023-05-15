#include "program/model/string_hash.hpp"
#include "program/model/value.hpp"
#include "vm/symbol_table.hpp"

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("Symbol Table should insert key successfully", "[SymbolTable]")
{
    SyncBlink::SymbolTable table;

    uint32_t hash = SyncBlink::stringHash("Test");
    table.set(hash, SyncBlink::Value(1.0f));

    SyncBlink::Value foundValue;
    REQUIRE(table.get(hash, &foundValue));
}

TEST_CASE("Symbol Table should adjust successfully", "[SymbolTable]")
{
    SyncBlink::SymbolTable table;

    for (int i = 0; i < 16; i++)
    {
        uint32_t hash = SyncBlink::stringHash("Test" + i);
        table.set(hash, SyncBlink::Value(1.0f));
    }
}