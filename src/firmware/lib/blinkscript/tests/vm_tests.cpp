#include "compiler/compiler.hpp"
#include "parser/ast/ast_node.hpp"
#include "parser/ast/program_ast.hpp"
#include "parser/model/token.hpp"
#include "parser/parser.hpp"
#include "parser/scanner/scanner.hpp"
#include "printer/disassembler.hpp"
#include "program/bytecode_loader.hpp"
#include "program/model/objects/native_function_object.hpp"
#include "program/model/objects/object.hpp"
#include "source/file_bytecode_source.hpp"
#include "source/string_script_source.hpp"
#include "vm/vm.hpp"
#include "program/model/value.hpp"

#include <catch2/catch.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

SyncBlink::Program compile(const std::string& script)
{
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    if (parser.hasError())
    {
        std::cout << "Line: " << std::get<0>(parser.getError()) << " - " << std::get<1>(parser.getError()) << '\n';
    }
    REQUIRE(!parser.hasError());

    auto compiler = SyncBlink::Compiler(source, programAst);
    auto program = compiler.compile();

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(!compiler.hasError());

    return program;
}

TEST_CASE("VM runs successfully", "[VM]")
{
    auto program = compile("12.012\n\"test\"");
    auto vm = SyncBlink::VM();

    vm.run(program);

    REQUIRE(vm.getStackSize() == 1);
}

TEST_CASE("VM runs let statements successfully", "[VM]")
{
    std::string script = "let x = true\n"
                         "let y = \"STRING\"\n"
                         "let k = 10\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10);
}

TEST_CASE("VM runs assign statements successfully", "[VM]")
{
    std::string script = "let k = \"lorem\"\n"
                         "k = \"ipsum\"\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::OBJECT);
    REQUIRE(static_cast<SyncBlink::StringObj*>(vm.getTop().object)->getString() == "ipsum");
}

TEST_CASE("VM runs bang prefix expression successfully", "[VM]")
{
    std::string script = "let k = false\n"
                         "k = !k\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::BOOL);
    REQUIRE(vm.getTop().boolean);
}

TEST_CASE("VM runs minus prefix expression successfully", "[VM]")
{
    std::string script = "let k = -2\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == -2);
}

TEST_CASE("VM returns error on bang non boolean", "[VM]")
{
    std::string script = "!2";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.hasError());
    REQUIRE(std::get<0>(vm.getError()) == 1);
}

TEST_CASE("VM returns error on negate non number", "[VM]")
{
    std::string script = "-false";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.hasError());
    REQUIRE(std::get<0>(vm.getError()) == 1);
}

TEST_CASE("VM runs calculations successfully", "[VM]")
{
    std::string script = "let k = 2 - 2 + 3 * 2 / 2\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 3);
}

TEST_CASE("VM runs number comparison successfully", "[VM]")
{
    std::string script = "let k = 2 <= 3\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::BOOL);
    REQUIRE(vm.getTop().boolean);
}

TEST_CASE("VM runs bool operations successfully", "[VM]")
{
    std::string script = "let k = true && false || true\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::BOOL);
    REQUIRE(vm.getTop().boolean);
}

TEST_CASE("VM runs string concat successfully", "[VM]")
{
    std::string script = "let k = \"lorem\" + \" ipsum\"\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::OBJECT);
    REQUIRE(vm.getTop().object->getType() == SyncBlink::ObjectType::STRING);
    REQUIRE(static_cast<SyncBlink::StringObj*>(vm.getTop().object)->getString() == "lorem ipsum");
}

TEST_CASE("VM runs string and number concat successfully", "[VM]")
{
    std::string script = "let k = \"lorem \" + 4\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::OBJECT);
    REQUIRE(vm.getTop().object->getType() == SyncBlink::ObjectType::STRING);
    REQUIRE(static_cast<SyncBlink::StringObj*>(vm.getTop().object)->getString() == "lorem 4");
}

TEST_CASE("VM returns error on wrong infix parameter", "[VM]")
{
    std::string script = "let k = 2 - false";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(vm.hasError());
}

TEST_CASE("VM runs grouped calculation successfully", "[VM]")
{
    std::string script = "let k = 20 - (2 + 3 * 2) / 1\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 12);
}

TEST_CASE("VM runs modulo successfully", "[VM]")
{
    std::string script = "let k = 5 % 2\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 1);
}

TEST_CASE("VM runs function variables successfully", "[VM]")
{
    std::string script = "let k = fun(x, y){ x + y }\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::OBJECT);
}

TEST_CASE("VM runs function call successfully", "[VM]")
{
    std::string script = "let r = 1\n"
                         "let z = 4\n"
                         "let k = fun(x, y){ r = x + y + z + r }\n"
                         "k(1, 2)\n"
                         "r";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 8);
}

TEST_CASE("VM runs condition expression successfully", "[VM]")
{
    std::string script = "let k = 0\n"
                         "if(k == 1) { k = 2 }\n"
                         "k";

    auto program = compile(script);

    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 0);
}

TEST_CASE("VM runs condition expression successfully (2)", "[VM]")
{
    std::string script = "let k = 0\n"
                         "if(k == 0) { k = 2 }\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM runs condition expression successfully (3)", "[VM]")
{
    std::string script = "let k = 2\n"
                         "if(k == 0) { k = 2 }\n"
                         "else { k = 3 }\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 3);
}

TEST_CASE("VM runs condition expression successfully (4)", "[VM]")
{
    std::string script = "let k = 2\n"
                         "if(k == 2) { k = 4 }\n"
                         "else { k = 3 }\n"
                         "k";

    auto program = compile(script);
    auto vm = SyncBlink::VM();

    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 4);
}

TEST_CASE("VM runs condition expression with shadowing successfully (5)", "[VM]")
{
    std::string script = "let x = 2\n"
                         "let k = 0\n"
                         "if(k == 0) { let x = 4 }\n"
                         "x";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM runs native function successfully", "[VM]")
{
    std::string script = "add(2.3, 2.2)";

    auto program = compile(script);
    auto vm = SyncBlink::VM();

    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>(
        [](std::vector<SyncBlink::Value> args) { return SyncBlink::Value(args[0].number + args[1].number); }, 2);

    vm.addNativeFun("add", nativeAddPtr);
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 4.5);
}

TEST_CASE("VM runs external function call successfully", "[VM]")
{
    std::string script = "let k = fun(x, y){ x / y }";

    auto program = compile(script);
    auto vm = SyncBlink::VM();

    std::vector<SyncBlink::Value> parameters;
    parameters.push_back(SyncBlink::Value(10.0f));
    parameters.push_back(SyncBlink::Value(2.0f));

    vm.run(program);
    vm.executeFun("k", parameters);

    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 5);
}

TEST_CASE("VM runs externaly called native function successfully", "[VM]")
{
    std::string script = "";

    auto program = compile(script);
    auto vm = SyncBlink::VM();

    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>(
        [](std::vector<SyncBlink::Value> args) { return SyncBlink::Value(args[0].number + args[1].number); }, 2);

    std::vector<SyncBlink::Value> parameters;
    parameters.push_back(SyncBlink::Value(10.0f));
    parameters.push_back(SyncBlink::Value(2.0f));

    vm.addNativeFun("add", nativeAddPtr);
    vm.run(program);
    vm.executeFun("add", parameters);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 12);
}

TEST_CASE("VM Block Scopes in child scopes (function -> further blocks) should not throw SEGFAULT", "[VM]")
{
    std::string script = "let colors = []\n"
                        "let update = fun(delta) {\n"
                        "if(vol == 0 || freq == 0 || vol < lVol) {\n"
                        "\n"
                        "}\n"
                        "minus(delta, 1.4)\n"
                        "}\n"
                        "\n"
                        "let init = fun(){\n"
                        "for(let i = nLedC - 1; i > 0; i = i - 1) {\n"
                        "colors[i] = xrgb(0,0,0)\n"
                        "}\n"
                        "if(nLedC == 16) {\n"
                        "setGroupsOf(4,4)\n"
                        "}\n"
                        "if(nLedC == 256) {\n"
                        "setLinearGroupsOf(16,4)\n"
                        "}\n"
                        "}\n";

    auto program = compile(script);

    auto vm = SyncBlink::VM();
    vm.run(program);

    auto nativeFunPtr = std::make_shared<SyncBlink::NativeFunObj>([](std::vector<SyncBlink::Value> args) { return SyncBlink::Value(args[0].number - 1); }, 2);
    vm.addNativeFun("minus", nativeFunPtr);
    vm.addNativeFun("xrgb", nativeFunPtr);
    vm.addNativeFun("setGroupsOf", nativeFunPtr);
    vm.addNativeFun("setLinearGroupsOf", nativeFunPtr);

    vm.setGlobal("lVol", SyncBlink::Value(0.0f));
    vm.setGlobal("lFreq", SyncBlink::Value(0.0f));
    vm.setGlobal("vol", SyncBlink::Value(0.0f));
    vm.setGlobal("freq", SyncBlink::Value(0.0f));
    vm.setGlobal("nLedC", SyncBlink::Value(2.0f));

    vm.executeFun("init", {});
    REQUIRE(!vm.hasError());

    std::vector<SyncBlink::Value> parameters;
    parameters.push_back(SyncBlink::Value(2.0f));

    vm.executeFun("update", parameters);
    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().number == 1.0f);

    std::vector<SyncBlink::Value> parameters2;
    parameters.push_back(SyncBlink::Value(1.0f));

    vm.executeFun("update", parameters);
    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().number == 0.0f);
}

TEST_CASE("VM executes array and indexing expressions successfully", "[VM]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]\n"
                         "testArray[3] = 2\n"
                         "testArray[3]";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM executes while loops successfully", "[VM]")
{
    std::string script = "let i = 0\n"
                         "let counter = 0\n"
                         "while(i < 10) {\n"
                         "let t = 9\n"
                         "counter = t + 1\n"
                         "i = i + 1\n"
                         "}\n"
                         "counter";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10);
}

TEST_CASE("VM executes loop called funcs without stack overflow", "[VM]")
{
    std::string script = "let i = 0\n"
                         "let f = 0\n"
                         "let add = fun(a, b) { a + b }\n"
                         "while(i < 10000) {\n"
                         "f = add(i, i)\n"
                         "i = i + 1\n"
                         "}\n"
                         "i";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10000);
}

TEST_CASE("VM executes loop called native funcs without stack overflow", "[VM]")
{
    std::string script = "let i = 0\n"
                         "let f = 0\n"
                         "while(i < 10000) {\n"
                         "f = add(i, i)\n"
                         "i = i + 1\n"
                         "}\n"
                         "i";

    auto program = compile(script);
    auto vm = SyncBlink::VM();

    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>(
        [](std::vector<SyncBlink::Value> args) { return SyncBlink::Value(args[0].number + args[1].number); }, 2);

    vm.addNativeFun("add", nativeAddPtr);
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10000);
}

TEST_CASE("VM executes for loops successfully", "[VM]")
{
    std::string script = "let count = 0\n"
                         "for(let i = 0; i < 10; i = i + 1) {\n"
                         "let count1 = count + 1\n"
                         "\tcount = count1\n"
                         "}\n"
                         "count";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10);
}

TEST_CASE("VM executes empty for loops successfully", "[VM]")
{
    std::string script = "for(let i = 0; i < 10; i = i + 1) {\n"
                         "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NIL);
}

TEST_CASE("VM executes multiple function calls successfully", "[VM]")
{
    std::string script = "let test = \"\"\n"
                         "let testFun = fun() {\n"
                         "test = \"Test\" + 3\n"
                         "test\n"
                         "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    vm.executeFun("testFun", {});
    vm.executeFun("testFun", {});

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::OBJECT);
    REQUIRE(vm.getTop().object->getType() == SyncBlink::ObjectType::STRING);
    REQUIRE(static_cast<SyncBlink::StringObj*>(vm.getTop().object)->getString() == "Test3");
}

// a3bfcf31ab1bedb98fa92f388f75125f95bd8e0f: ~7.5secs
// 2ddd7f99780ef8cfb8c84215aa4511a91eaf4771: ~4.5secs
// 984ece8e9812a628aa6029c889454b0e2b1d61ad: ~2.6 sec
// now WITH assignments: ~2.75sec
TEST_CASE("VM executes array in loops successfully (for performance test)", "[VM]")
{
    std::string script = "let test = []\n"
                         "let testFun = fun() {\n"
                         "for(let j = 500000; j > 0; j = j - 1) {\n"
                         "let k = 1\n"
                         "test[j] = test[j-1]\n"
                         "test[1]\n"
                         "}}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);
    vm.executeFun("testFun", {});

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NIL);
}

// 984ece8e9812a628aa6029c889454b0e2b1d61ad: ~6.25 sec
// now: ~2.2 sec
TEST_CASE("VM executes functions with call in loops successfully (for performance test)", "[VM]")
{
    std::string script = "let test = []\n"
                         "let j = 2\n"
                         "let testFun = fun() {\n"
                         "test[0] = j }\n"
                         "for(let j = 500000; j > 0; j = j - 1) {\n"
                         "testFun()\n"
                         "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
}

TEST_CASE("VM executes inline expressions successfully", "[VM]")
{
    std::string script = "let count = if(false) { 1 } else { 0 }\n"
                         "count";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 0);
}

TEST_CASE("VM executes inline expressions successfully (2)", "[VM]")
{
    std::string script = "let inline = 2\n"
                         "let count = 2 + (inline = 5)\n"
                         "inline";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 5);
}

TEST_CASE("VM return correct return values, if last statement is a block", "[VM]")
{
    std::string script = "let x = 0\n"
                        "if(x == 0) {\n"
                            "let y = 2\n"
                            "y\n"
                        "}";

    auto program = compile(script);

    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM return correct return values, if last statement is a block (2)", "[VM]")
{
    std::string script = "let x = 0\n"
                        "if(x == 1) {\n"
                            "let y = 2\n"
                            "y\n"
                        "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NIL);
}

TEST_CASE("VM return correct return values, if last statement is a block (3)", "[VM]")
{
    std::string script = "let x = 0\n"
                        "for(let y = 0; y < 5; y = y + 1) {\n"
                            "let z = 2\n"
                            "x = z + y\n"
                        "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 6);
}

TEST_CASE("VM return correct return values, if last statement is a block (4)", "[VM]")
{
    std::string script = "let x = 5\n"
                        "while(x > 0) {\n"
                            "let z = x\n"
                            "x = x - 1\n"
                        "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 0);
}

TEST_CASE("VM loads correct local values", "[VM]")
{
    std::string script = "let test = fun(x) {\n"
                            "let y = 2\n"
                            "x + y\n"
                        "}\n"
                        "test(2)\n"
                        "let x = 5\n"
                        "if(x == 5){\n"
                            "let u = 3"
                            "test(x + u)\n"
                        "}";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10);
}

TEST_CASE("VM uses foreign locals successfully", "[VM]")
{
    std::string script = "let add = fun(x, y){\n"
                            "x + y\n"
                        "}\n"
                        "let test = fun(x) {\n"
                            "fun(y) {\n"
                                "add(x, y)\n"
                            "}\n"
                        "}\n"
                        "let add2 = test(2)\n"
                        "add2(4)";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 6);
}


 TEST_CASE("VM executes loaded snake script", "[VM]")
{
    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/Snake.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    auto vm = SyncBlink::VM();
    vm.run(program);
    REQUIRE(!vm.hasError());
    REQUIRE(vm.getStackSize() == 1);
}