#include "compiler/compiler.hpp"
#include "parser/parser.hpp"
#include "printer/disassembler.hpp"
#include "program/bytecode_loader.hpp"
#include "program/model/objects/function_object.hpp"
#include "source/file_bytecode_source.hpp"
#include "source/string_script_source.hpp"

#include <catch2/catch.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

namespace ByteCodeParserTests
{
    SyncBlink::ProgramAst parsec(SyncBlink::Parser& parser)
    {
        auto programAst = parser.parse();
        if (parser.hasError())
        {
            std::cout << "Line: " << std::get<0>(parser.getError()) << " - " << std::get<1>(parser.getError()) << '\n';
        }
        REQUIRE(!parser.hasError());

        return programAst;
    }

    void testByteCode(std::shared_ptr<SyncBlink::FileByteCodeSource> byteCodeSource, std::string script)
    {
        auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
        auto loadedProgram = byteCodeLoader.getProgram();

        auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
        auto parser = SyncBlink::Parser(source);
        auto programAst = parsec(parser);

        auto compiler = SyncBlink::Compiler(source, programAst);
        auto compiledProgram = compiler.compile();

        SyncBlink::Disassembler dis;
        auto disassembledCompiledCode = dis.print(compiledProgram);
        auto disassembledLoadedCode = dis.print(loadedProgram);

        REQUIRE(disassembledCompiledCode == disassembledLoadedCode);
    }
}

TEST_CASE("Bytecode loads let statements successfully", "[bytecodeloader]")
{
    std::string script = "let x = true\n"
                         "let y = \"STRING\"\n"
                         "let k = 10\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test1.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads assign statements successfully", "[bytecodeloader]")
{
    std::string script = "let k = \"lorem\"\n"
                         "k = \"ipsum\"\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test2.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads bang prefix expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = false\n"
                         "k = !k\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test3.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads minus prefix expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = -2\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test4.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads calculations successfully", "[bytecodeloader]")
{
    std::string script = "let k = 2 - 2 + 3 * 2 / 2\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test5.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads number comparison successfully", "[bytecodeloader]")
{
    std::string script = "let k = 2 <= 3\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test6.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads bool operations successfully", "[bytecodeloader]")
{
    std::string script = "let k = true && false || true\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test7.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads string concat successfully", "[bytecodeloader]")
{
    std::string script = "let k = \"lorem\" + \" ipsum\"\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test8.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads grouped calculation successfully", "[bytecodeloader]")
{
    std::string script = "let k = 20 - (2 + 3 * 2) / 1\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test9.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads function variables successfully", "[bytecodeloader]")
{
    std::string script = "let k = fun(x, y){ x + y }\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test10.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads function call successfully", "[bytecodeloader]")
{
    std::string script = "let r = 1\n"
                         "let z = 4\n"
                         "let k = fun(x, y){ r = x + y + z + r }\n"
                         "k(1, 2)\n"
                         "r";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test11.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads function call successfully (2)", "[bytecodeloader]")
{
    std::string script = "let k = fun(x, y){ x / y }\n"
                         "k(100, 2)";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test12.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads condition expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = 2\n"
                         "if(k == 0) { k = 2 }\n"
                         "else { k = 3 }\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test13.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads script successfully", "[bytecodeloader]")
{
    std::string script = "let update = fun(delta) {\n"
                         "if(vol == 0 || freq == 0 || vol < lVol) {\n"
                         "\n"
                         "}\n"
                         "setAllLeds(12255470)\n"
                         "}\n"
                         "\n"
                         "let init = fun(){}\n"
                         "let scriptName = \"simpleScript\"";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test14.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads array and indexing expressions successfully", "[bytecodeloader]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]\n"
                         "testArray[0]";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test15.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads while loops successfully", "[bytecodeloader]")
{
    std::string script = "let i = 0\n"
                         "let counter = 0\n"
                         "while(i < 10) {\n"
                         "counter = counter + 1\n"
                         "i = i + 1\n"
                         "}\n"
                         "counter";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test16.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads for loops successfully", "[bytecodeloader]")
{
    std::string script = "let count = 0\n"
                         "for(let i = 0; i < 10; i = i + 1) {\n"
                         "let count1 = count + 1\n"
                         "\tcount = count1\n"
                         "}\n"
                         "count";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test17.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}

TEST_CASE("Bytecode loads Snake script successfully", "[bytecodeloader]")
{
    std::string script = "let h=0\n"
                         "let s=0\n"
                         "let v=0\n"
                         "let colors = []\n"
                         "\n"
                         "let update = fun(delta) {\n"
                         "if(vol == 0 || freq == 0 || (vol < lVol * 1.05 && v > 0.25)){\n"
                         "if(v > 0.025){ v = v - 0.025 }\n"
                         "else{ v = 0 }\n"
                         "} else {\n"
                         "// To make the effects more colorful\n"
                         "if(freq > maxF/2) {\n"
                         "freq = maxF/2\n"
                         "}\n"
                         "h = map(freq, 100, maxF/2, 240, 0)\n"
                         "s = 1\n"
                         "v = map(vol, 0, 100, 0, 1)\n"
                         "}\n"
                         "\n"
                         "for(let i = nLedC - 1; i > 0; i = i - 1) {\n"
                         "colors[i] = colors[i-1]\n"
                         "}\n"
                         "colors[0] = xhsv(h, s, v)\n"
                         "setLeds(colors)\n"
                         "}\n"
                         "\n"
                         "let init = fun(){\n"
                         "for(let i = nLedC - 1; i > 0; i = i - 1) {\n"
                         "colors[i] = xrgb(0,0,0)\n"
                         "}\n"
                         "if(nLedC == 16) {\n"
                         "setGroupsOf(4)\n"
                         "}\n"
                         "if(nLedC == 256) {\n"
                         "setLinearGroupsOf(16)\n"
                         "}\n"
                         "}";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/Snake.b");
    ByteCodeParserTests::testByteCode(byteCodeSource, script);
}