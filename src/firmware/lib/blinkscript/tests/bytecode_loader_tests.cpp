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
}

TEST_CASE("Bytecode loads let statements successfully", "[bytecodeloader]")
{
    std::string script = "let x = true\n"
                         "let y = \"STRING\"\n"
                         "let k = 10\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test1.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 4\n\n"
                                ".constants:\n"
                                "\t@  0: TRUE\n"
                                "\t@  1: \"x\"\n"
                                "\t@  2: \"STRING\"\n"
                                "\t@  3: \"y\"\n"
                                "\t@  4: 10\n"
                                "\t@  5: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: DEFINE\n"
                                "\t  7: .3\n"
                                "\t  8: VALUE\n"
                                "\t  9: .4\n"
                                "\t 10: DEFINE\n"
                                "\t 11: .5\n"
                                "\t 12: LOAD\n"
                                "\t 13: .5\n");
}

TEST_CASE("Bytecode loads assign statements successfully", "[bytecodeloader]")
{
    std::string script = "let k = \"lorem\"\n"
                         "k = \"ipsum\"\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test2.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 3\n\n"
                                ".constants:\n"
                                "\t@  0: \"lorem\"\n"
                                "\t@  1: \"k\"\n"
                                "\t@  2: \"ipsum\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: SET\n"
                                "\t  7: .1\n"
                                "\t  8: LOAD\n"
                                "\t  9: .1\n");
}

TEST_CASE("Bytecode loads bang prefix expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = false\n"
                         "k = !k\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test3.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: FALSE\n"
                                "\t@  1: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: LOAD\n"
                                "\t  5: .1\n"
                                "\t  6: BANG_NEGATE\n"
                                "\t  7: SET\n"
                                "\t  8: .1\n"
                                "\t  9: LOAD\n"
                                "\t 10: .1\n");
}

TEST_CASE("Bytecode loads minus prefix expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = -2\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test4.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: 2\n"
                                "\t@  1: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: NEGATE\n"
                                "\t  3: DEFINE\n"
                                "\t  4: .1\n"
                                "\t  5: LOAD\n"
                                "\t  6: .1\n");
}

TEST_CASE("Bytecode loads calculations successfully", "[bytecodeloader]")
{
    std::string script = "let k = 2 - 2 + 3 * 2 / 2\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test5.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: 2\n"
                                "\t@  1: 3\n"
                                "\t@  2: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: VALUE\n"
                                "\t  3: .0\n"
                                "\t  4: SUB\n"
                                "\t  5: VALUE\n"
                                "\t  6: .1\n"
                                "\t  7: VALUE\n"
                                "\t  8: .0\n"
                                "\t  9: MUL\n"
                                "\t 10: VALUE\n"
                                "\t 11: .0\n"
                                "\t 12: DIV\n"
                                "\t 13: ADD\n"
                                "\t 14: DEFINE\n"
                                "\t 15: .2\n"
                                "\t 16: LOAD\n"
                                "\t 17: .2\n");
}

TEST_CASE("Bytecode loads number comparison successfully", "[bytecodeloader]")
{
    std::string script = "let k = 2 <= 3\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test6.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: 2\n"
                                "\t@  1: 3\n"
                                "\t@  2: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: VALUE\n"
                                "\t  3: .1\n"
                                "\t  4: LESS_EQUAL\n"
                                "\t  5: DEFINE\n"
                                "\t  6: .2\n"
                                "\t  7: LOAD\n"
                                "\t  8: .2\n");
}

TEST_CASE("Bytecode loads bool operations successfully", "[bytecodeloader]")
{
    std::string script = "let k = true && false || true\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test7.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: TRUE\n"
                                "\t@  1: FALSE\n"
                                "\t@  2: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: VALUE\n"
                                "\t  3: .1\n"
                                "\t  4: AND\n"
                                "\t  5: VALUE\n"
                                "\t  6: .0\n"
                                "\t  7: OR\n"
                                "\t  8: DEFINE\n"
                                "\t  9: .2\n"
                                "\t 10: LOAD\n"
                                "\t 11: .2\n");
}

TEST_CASE("Bytecode loads string concat successfully", "[bytecodeloader]")
{
    std::string script = "let k = \"lorem\" + \" ipsum\"\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test8.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 3\n\n"
                                ".constants:\n"
                                "\t@  0: \"lorem\"\n"
                                "\t@  1: \" ipsum\"\n"
                                "\t@  2: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: VALUE\n"
                                "\t  3: .1\n"
                                "\t  4: ADD\n"
                                "\t  5: DEFINE\n"
                                "\t  6: .2\n"
                                "\t  7: LOAD\n"
                                "\t  8: .2\n");
}

TEST_CASE("Bytecode loads grouped calculation successfully", "[bytecodeloader]")
{
    std::string script = "let k = 20 - (2 + 3 * 2) / 1\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test9.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: 20\n"
                                "\t@  1: 2\n"
                                "\t@  2: 3\n"
                                "\t@  3: 1\n"
                                "\t@  4: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: VALUE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: VALUE\n"
                                "\t  7: .1\n"
                                "\t  8: MUL\n"
                                "\t  9: ADD\n"
                                "\t 10: VALUE\n"
                                "\t 11: .3\n"
                                "\t 12: DIV\n"
                                "\t 13: SUB\n"
                                "\t 14: DEFINE\n"
                                "\t 15: .4\n"
                                "\t 16: LOAD\n"
                                "\t 17: .4\n");
}

TEST_CASE("Bytecode loads function variables successfully", "[bytecodeloader]")
{
    std::string script = "let k = fun(x, y){ x + y }\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test10.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 2\n\n"
                                ".constants:\n"
                                "\t@  0: FUN\n"
                                "\t@  1: \"k\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: LOAD\n"
                                "\t  5: .1\n");

    REQUIRE(program.getConstant(0).getType() == SyncBlink::ValueType::OBJECT);

    auto funObj = static_cast<SyncBlink::FunObj*>(program.getConstant(0).object);
    disassembledCode = dis.print(funObj->getProgram());
    REQUIRE(funObj->getParameters().size() == 2);
    REQUIRE(disassembledCode == ".object.count 2\n\n"
                                ".constants:\n"
                                "\t@  0: \"x\"\n"
                                "\t@  1: \"y\"\n\n"
                                ".code:\n"
                                "\t  0: FRAME\n"
                                "\t  1: LOAD\n"
                                "\t  2: .0\n"
                                "\t  3: LOAD\n"
                                "\t  4: .1\n"
                                "\t  5: ADD\n"
                                "\t  6: UNFRAME\n");
}

TEST_CASE("Bytecode loads function call successfully", "[bytecodeloader]")
{
    std::string script = "let z = 4\n"
                         "let k = fun(x, y){ x + y + z }\n"
                         "k(1, 2)";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test11.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 3\n\n"
                                ".constants:\n"
                                "\t@  0: 4\n"
                                "\t@  1: \"z\"\n"
                                "\t@  2: FUN\n"
                                "\t@  3: \"k\"\n"
                                "\t@  4: 2\n"
                                "\t@  5: 1\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: DEFINE\n"
                                "\t  7: .3\n"
                                "\t  8: VALUE\n"
                                "\t  9: .4\n"
                                "\t 10: VALUE\n"
                                "\t 11: .5\n"
                                "\t 12: CALL\n"
                                "\t 13: .3\n");
}

TEST_CASE("Bytecode loads function call successfully (2)", "[bytecodeloader]")
{
    std::string script = "let k = fun(x, y){ x / y }\n"
                         "k(100, 2)";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test12.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 2\n\n"
                                ".constants:\n"
                                "\t@  0: FUN\n"
                                "\t@  1: \"k\"\n"
                                "\t@  2: 2\n"
                                "\t@  3: 100\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: VALUE\n"
                                "\t  7: .3\n"
                                "\t  8: CALL\n"
                                "\t  9: .1\n");
}

TEST_CASE("Bytecode loads condition expression successfully", "[bytecodeloader]")
{
    std::string script = "let k = 0\n"
                         "if(k == 0) { k = 2 }\n"
                         "k";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test13.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 1\n\n"
                                ".constants:\n"
                                "\t@  0: 0\n"
                                "\t@  1: \"k\"\n"
                                "\t@  2: 18\n"
                                "\t@  3: 2\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: LOAD\n"
                                "\t  5: .1\n"
                                "\t  6: VALUE\n"
                                "\t  7: .0\n"
                                "\t  8: EQUAL_EQUAL\n"
                                "\t  9: JMP_NOT\n"
                                "\t 10: VALUE\n"
                                "\t 11: .2\n"
                                "\t 12: FRAME\n"
                                "\t 13: VALUE\n"
                                "\t 14: .3\n"
                                "\t 15: SET\n"
                                "\t 16: .1\n"
                                "\t 17: UNFRAME\n"
                                "\t 18: LOAD\n"
                                "\t 19: .1\n");
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
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 6\n\n"
                                ".constants:\n"
                                "\t@  0: FUN\n"
                                "\t@  1: \"update\"\n"
                                "\t@  2: FUN\n"
                                "\t@  3: \"init\"\n"
                                "\t@  4: \"simpleScript\"\n"
                                "\t@  5: \"scriptName\"\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .2\n"
                                "\t  6: DEFINE\n"
                                "\t  7: .3\n"
                                "\t  8: VALUE\n"
                                "\t  9: .4\n"
                                "\t 10: DEFINE\n"
                                "\t 11: .5\n");
}

TEST_CASE("Bytecode loads array and indexing expressions successfully", "[bytecodeloader]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]\n"
                         "testArray[0]";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test15.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 4\n\n"
                                ".constants:\n"
                                "\t@  0: 1\n"
                                "\t@  1: \"lorem\"\n"
                                "\t@  2: FUN\n"
                                "\t@  3: [1, \"lorem\", FUN]\n"
                                "\t@  4: \"testArray\"\n"
                                "\t@  5: 0\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .3\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .4\n"
                                "\t  4: LOAD\n"
                                "\t  5: .4\n"
                                "\t  6: VALUE\n"
                                "\t  7: .5\n"
                                "\t  8: INDEX\n");
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
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 2\n\n"
                                ".constants:\n"
                                "\t@  0: 0\n"
                                "\t@  1: \"i\"\n"
                                "\t@  2: \"counter\"\n"
                                "\t@  3: 10\n"
                                "\t@  4: 34\n"
                                "\t@  5: 1\n"
                                "\t@  6: 9\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: VALUE\n"
                                "\t  5: .0\n"
                                "\t  6: DEFINE\n"
                                "\t  7: .2\n"
                                "\t  8: FRAME\n"
                                "\t  9: LOAD\n"
                                "\t 10: .1\n"
                                "\t 11: VALUE\n"
                                "\t 12: .3\n"
                                "\t 13: LESS\n"
                                "\t 14: JMP_NOT\n"
                                "\t 15: VALUE\n"
                                "\t 16: .4\n"
                                "\t 17: LOAD\n"
                                "\t 18: .2\n"
                                "\t 19: VALUE\n"
                                "\t 20: .5\n"
                                "\t 21: ADD\n"
                                "\t 22: SET\n"
                                "\t 23: .2\n"
                                "\t 24: LOAD\n"
                                "\t 25: .1\n"
                                "\t 26: VALUE\n"
                                "\t 27: .5\n"
                                "\t 28: ADD\n"
                                "\t 29: SET\n"
                                "\t 30: .1\n"
                                "\t 31: JMP\n"
                                "\t 32: VALUE\n"
                                "\t 33: .6\n"
                                "\t 34: UNFRAME\n"
                                "\t 35: LOAD\n"
                                "\t 36: .2\n");
}

TEST_CASE("Bytecode loads for loops successfully", "[bytecodeloader]")
{
    std::string script = "let count = 0\n"
                         "for(let i = 0; i < 10; i = i + 1) {\n"
                         "\tcount = count + 1\n"
                         "}\n"
                         "count";

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/test17.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(disassembledCode == ".object.count 2\n\n"
                                ".constants:\n"
                                "\t@  0: 0\n"
                                "\t@  1: \"count\"\n"
                                "\t@  2: \"i\"\n"
                                "\t@  3: 10\n"
                                "\t@  4: 34\n"
                                "\t@  5: 1\n"
                                "\t@  6: 9\n\n"
                                ".code:\n"
                                "\t  0: VALUE\n"
                                "\t  1: .0\n"
                                "\t  2: DEFINE\n"
                                "\t  3: .1\n"
                                "\t  4: FRAME\n"
                                "\t  5: VALUE\n"
                                "\t  6: .0\n"
                                "\t  7: DEFINE\n"
                                "\t  8: .2\n"
                                "\t  9: LOAD\n"
                                "\t 10: .2\n"
                                "\t 11: VALUE\n"
                                "\t 12: .3\n"
                                "\t 13: LESS\n"
                                "\t 14: JMP_NOT\n"
                                "\t 15: VALUE\n"
                                "\t 16: .4\n"
                                "\t 17: LOAD\n"
                                "\t 18: .1\n"
                                "\t 19: VALUE\n"
                                "\t 20: .5\n"
                                "\t 21: ADD\n"
                                "\t 22: SET\n"
                                "\t 23: .1\n"
                                "\t 24: LOAD\n"
                                "\t 25: .2\n"
                                "\t 26: VALUE\n"
                                "\t 27: .5\n"
                                "\t 28: ADD\n"
                                "\t 29: SET\n"
                                "\t 30: .2\n"
                                "\t 31: JMP\n"
                                "\t 32: VALUE\n"
                                "\t 33: .6\n"
                                "\t 34: UNFRAME\n"
                                "\t 35: LOAD\n"
                                "\t 36: .1\n");
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

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = ByteCodeParserTests::parsec(parser);

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto compiledProgram = compiler.compile();

    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/Snake.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto loadedProgram = byteCodeLoader.getProgram();

    SyncBlink::Disassembler dis;
    auto disassembledCompiledCode = dis.print(compiledProgram);
    auto disassembledLoadedCode = dis.print(loadedProgram);

    REQUIRE(disassembledCompiledCode == disassembledLoadedCode);

    auto compiledFunObj = static_cast<SyncBlink::FunObj*>(compiledProgram.getConstant(6).object);
    auto loadedFunObj = static_cast<SyncBlink::FunObj*>(loadedProgram.getConstant(6).object);

    disassembledCompiledCode = dis.print(compiledFunObj->getProgram());
    disassembledLoadedCode = dis.print(loadedFunObj->getProgram());

    REQUIRE(disassembledCompiledCode == disassembledLoadedCode);

    compiledFunObj = static_cast<SyncBlink::FunObj*>(compiledProgram.getConstant(8).object);
    loadedFunObj = static_cast<SyncBlink::FunObj*>(loadedProgram.getConstant(8).object);

    disassembledCompiledCode = dis.print(compiledFunObj->getProgram());
    disassembledLoadedCode = dis.print(loadedFunObj->getProgram());

    REQUIRE(disassembledCompiledCode == disassembledLoadedCode);
}