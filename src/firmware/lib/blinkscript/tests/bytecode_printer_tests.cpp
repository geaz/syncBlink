#include "compiler/compiler.hpp"
#include "parser/ast/ast_node.hpp"
#include "parser/ast/program_ast.hpp"
#include "parser/model/token.hpp"
#include "parser/parser.hpp"
#include "parser/scanner/scanner.hpp"
#include "printer/bytecode_printer.hpp"
#include "program/bytecode_loader.hpp"
#include "program/model/objects/function_object.hpp"
#include "source/file_bytecode_source.hpp"
#include "source/string_script_source.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace ByteCodePrinterTests
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

    /* https://stackoverflow.com/a/37575457 */
    bool compareFiles(const std::string& p1, const std::string& p2)
    {
        std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
        std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

        if (f1.fail() || f2.fail())
        {
            return false; // file problem
        }

        if (f1.tellg() != f2.tellg())
        {
            return false; // size mismatch
        }

        // seek back to beginning and use std::equal to compare contents
        f1.seekg(0, std::ifstream::beg);
        f2.seekg(0, std::ifstream::beg);
        return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()), std::istreambuf_iterator<char>(),
                          std::istreambuf_iterator<char>(f2.rdbuf()));
    }
}

TEST_CASE("Bytecode printer converts let statements successfully", "[bytecodeprinter]")
{
    std::string script = "let x = true\n"
                         "let y = \"STRING\"\n"
                         "let k = 10\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test1.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test1.b", "../tests/bytecodes/test1.b"));
}

TEST_CASE("Bytecode printer converts assign statements successfully", "[bytecodeprinter]")
{
    std::string script = "let k = \"lorem\"\n"
                         "k = \"ipsum\"\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test2.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test2.b", "../tests/bytecodes/test2.b"));
}

TEST_CASE("Bytecode prints bang prefix expression successfully", "[bytecodeprinter]")
{
    std::string script = "let k = false\n"
                         "k = !k\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test3.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test3.b", "../tests/bytecodes/test3.b"));
}

TEST_CASE("Bytecode prints minus prefix expression successfully", "[bytecodeprinter]")
{
    std::string script = "let k = -2\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test4.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test4.b", "../tests/bytecodes/test4.b"));
}

TEST_CASE("Bytecode prints calculations successfully", "[bytecodeprinter]")
{
    std::string script = "let k = 2 - 2 + 3 * 2 / 2\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test5.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test5.b", "../tests/bytecodes/test5.b"));
}

TEST_CASE("Bytecode prints number comparison successfully", "[bytecodeprinter]")
{
    std::string script = "let k = 2 <= 3\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test6.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test6.b", "../tests/bytecodes/test6.b"));
}

TEST_CASE("Bytecode prints bool operations successfully", "[bytecodeprinter]")
{
    std::string script = "let k = true && false || true\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test7.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test7.b", "../tests/bytecodes/test7.b"));
}

TEST_CASE("Bytecode prints string concat successfully", "[bytecodeprinter]")
{
    std::string script = "let k = \"lorem\" + \" ipsum\"\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test8.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test8.b", "../tests/bytecodes/test8.b"));
}

TEST_CASE("Bytecode prints grouped calculation successfully", "[bytecodeprinter]")
{
    std::string script = "let k = 20 - (2 + 3 * 2) / 1\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test9.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test9.b", "../tests/bytecodes/test9.b"));
}

TEST_CASE("Bytecode prints function variables successfully", "[bytecodeprinter]")
{
    std::string script = "let k = fun(x, y){ x + y }\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test10.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test10.b", "../tests/bytecodes/test10.b"));
}

TEST_CASE("Bytecode prints function call successfully", "[bytecodeprinter]")
{
    std::string script = "let z = 4\n"
                         "let k = fun(x, y){ x + y + z }\n"
                         "k(1, 2)";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test11.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test11.b", "../tests/bytecodes/test11.b"));
}

TEST_CASE("Bytecode prints function call successfully (2)", "[bytecodeprinter]")
{
    std::string script = "let k = fun(x, y){ x / y }\n"
                         "k(100, 2)";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test12.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test12.b", "../tests/bytecodes/test12.b"));
}

TEST_CASE("Bytecode prints condition expression successfully", "[bytecodeprinter]")
{
    std::string script = "let k = 0\n"
                         "if(k == 0) { k = 2 }\n"
                         "k";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test13.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test13.b", "../tests/bytecodes/test13.b"));
}

TEST_CASE("Bytecode prints script successfully", "[bytecodeprinter]")
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

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test14.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test14.b", "../tests/bytecodes/test14.b"));
}

TEST_CASE("Bytecode prints array and indexing expressions successfully", "[bytecodeprinter]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]\n"
                         "testArray[0]";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test15.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test15.b", "../tests/bytecodes/test15.b"));
}

TEST_CASE("Bytecode prints while loops successfully", "[bytecodeprinter]")
{
    std::string script = "let i = 0\n"
                         "let counter = 0\n"
                         "while(i < 10) {\n"
                         "counter = counter + 1\n"
                         "i = i + 1\n"
                         "}\n"
                         "counter";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test16.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test16.b", "../tests/bytecodes/test16.b"));
}

TEST_CASE("Bytecode prints for loops successfully", "[bytecodeprinter]")
{
    std::string script = "let count = 0\n"
                         "for(let i = 0; i < 10; i = i + 1) {\n"
                         "\tcount = count + 1\n"
                         "}\n"
                         "count";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("test17.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("test17.b", "../tests/bytecodes/test17.b"));
}

TEST_CASE("Bytecode prints Snake script successfully", "[bytecodeprinter]")
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

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodePrinterTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::ByteCodePrinter bcp;
    auto byteCode = bcp.print(program);

    std::ofstream byteCodeFile("Snake.b", std::ios::out | std::ios::binary);
    byteCodeFile.write((char*)&byteCode[0], byteCode.size());
    byteCodeFile.close();

    REQUIRE(!compiler.hasError());
    REQUIRE(ByteCodePrinterTests::compareFiles("Snake.b", "../tests/bytecodes/Snake.b"));
}