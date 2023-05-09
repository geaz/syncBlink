#include <catch2/catch.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

#include "parser/scanner/scanner.hpp"
#include "source/string_script_source.hpp"
#include "parser/model/token.hpp"
#include "parser/parser.hpp"
#include "parser/ast/ast_node.hpp"
#include "parser/ast/program_ast.hpp"
#include "printer/disassembler.hpp"
#include "program/model/objects/object.hpp"
#include "program/model/objects/native_function_object.hpp"
#include "compiler/compiler.hpp"
#include "source/file_bytecode_source.hpp"
#include "program/bytecode_loader.hpp"
#include "vm/vm.hpp"

SyncBlink::Program compile(const std::string& script)
{
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    if(parser.hasError())
    {
        std::cout << "Line: " << std::get<0>(parser.getError()) << " - " << std::get<1>(parser.getError()) << '\n';
    }
    REQUIRE(!parser.hasError());

    auto compiler = SyncBlink::Compiler(source, programAst);
    return compiler.compile();
}

TEST_CASE("VM runs successfully", "[VM]")
{
    auto program = compile("12.012\n\"test\"");
    auto vm = SyncBlink::VM();

    vm.run(program);
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
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 12);
}

TEST_CASE("VM runs function variables successfully", "[VM]")
{
    std::string script = "let k = fun(x, y){ x + y }\n"
                         "k";
    
    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
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

    SyncBlink::Disassembler dis;
    auto disassembledCode = dis.print(program);

    REQUIRE(!vm.hasError());
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
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM runs native function successfully", "[VM]")
{
    std::string script = "add(2, 2)";

    auto program = compile(script);
    auto vm = SyncBlink::VM();
    
    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>([](SyncBlink::Frame frame)
    {
        return SyncBlink::Value(frame.get("arg0").number + frame.get("arg1").number); 
    }, 2);

    vm.addNativeFun("add", nativeAddPtr);
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 4);
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
    
    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>([](SyncBlink::Frame frame)
    {
        return SyncBlink::Value(frame.get("arg0").number + frame.get("arg1").number); 
    }, 2);

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
    std::string script = "let update = fun(delta) {\n"
                         "if(vol == 0 || freq == 0 || vol < lVol) {\n"
                         "\n"   
                         "}\n"
                         "setAllLeds(12255470)\n"	
                         "}\n"
                         "\n"
                         "let init = fun(){}\n"
                         "let scriptName = \"simpleScript\"";
    
    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    auto nativeFunPtr = std::make_shared<SyncBlink::NativeFunObj>([](SyncBlink::Frame frame)
    {
        return SyncBlink::Value();
    }, 1);
    vm.addNativeFun("setAllLeds", nativeFunPtr);

    vm.getFrame().addSet("lVol", SyncBlink::Value(0.0f));
    vm.getFrame().addSet("lFreq", SyncBlink::Value(0.0f));
    vm.getFrame().addSet("vol", SyncBlink::Value(0.0f));
    vm.getFrame().addSet("freq", SyncBlink::Value(0.0f));

    std::vector<SyncBlink::Value> parameters;
    parameters.push_back(SyncBlink::Value(0.0f));

    vm.executeFun("update", parameters);
    REQUIRE(!vm.hasError());

    std::vector<SyncBlink::Value> parameters2;
    parameters.push_back(SyncBlink::Value(0.0f));

    vm.executeFun("update", parameters);
    REQUIRE(!vm.hasError());
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
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 2);
}

TEST_CASE("VM executes while loops successfully", "[VM]")
{
    std::string script = "let i = 0\n"
                         "let counter = 0\n"
                         "while(i < 10) {\n"
                         "counter = counter + 1\n"
                         "i = i + 1\n"
                         "}\n"
                         "counter";
    
    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
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
    
    auto nativeAddPtr = std::make_shared<SyncBlink::NativeFunObj>([](SyncBlink::Frame frame)
    {
        return SyncBlink::Value(frame.get("arg0").number + frame.get("arg1").number); 
    }, 2);

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
                         "\tcount = count + 1\n"
                         "}\n"
                         "count";
    
    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NUMBER);
    REQUIRE(vm.getTop().number == 10);
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
// ?: ~3.0secs
TEST_CASE("VM executes array in loops successfully (for performance test)", "[VM]")
{
    std::string script = "let test = []\n"
                         "let testFun = fun() {\n"
                         "for(let j = 500000; j > 0; j = j - 1) {\n"
                         "test[j] = test[j-1]\n"
                         "test[1]\n"
                         "}}";
    
    auto program = compile(script);
    auto vm = SyncBlink::VM();
    vm.run(program);    
    vm.executeFun("testFun", {});

    REQUIRE(!vm.hasError());
    REQUIRE(vm.getTop().getType() == SyncBlink::ValueType::NIL);
}

TEST_CASE("VM executes loaded snake script", "[VM]")
{
    auto byteCodeSource = std::make_shared<SyncBlink::FileByteCodeSource>("../tests/bytecodes/Snake.b");
    auto byteCodeLoader = SyncBlink::ByteCodeLoader(byteCodeSource);
    auto program = byteCodeLoader.getProgram();

    auto vm = SyncBlink::VM();
    vm.run(program);
    REQUIRE(!vm.hasError());
}