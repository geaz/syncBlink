#include <catch2/catch.hpp>
#include "vm/model/value.hpp"
#include "vm/model/symbol_table.hpp"
#include "vm/model/string_hash.hpp"
#include <iostream>

TEST_CASE("Should insert key successfully", "[SymbolTable]")
{
    SyncBlink::SymbolTable table;
    
    uint32_t hash = SyncBlink::stringHash("Test");
    table.set(hash, SyncBlink::Value(1.0f));

    SyncBlink::Value foundValue;
    REQUIRE(table.get(hash, &foundValue));
}

TEST_CASE("Should adjust successfully", "[SymbolTable]")
{
    SyncBlink::SymbolTable table;

    for(int i = 0; i < 16; i++)
    {        
        uint32_t hash = SyncBlink::stringHash("Test" + i);
        table.set(hash, SyncBlink::Value(1.0f));
    }
}