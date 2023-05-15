#include "parser/ast/ast_node.hpp"
#include "parser/ast/program_ast.hpp"
#include "parser/model/token.hpp"
#include "parser/parser.hpp"
#include "parser/scanner/scanner.hpp"
#include "printer/script_printer.hpp"
#include "source/string_script_source.hpp"

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("Parser reports error on unsupported syntax", "[parse]")
{
    auto source = std::make_shared<SyncBlink::StringScriptSource>(std::string("let k k = \"WTF?\""));
    auto parser = SyncBlink::Parser(source);
    parser.parse();

    REQUIRE(parser.hasError());
}

TEST_CASE("Parser parses expression statements succesfully", "[parse]")
{
    std::string script = "4 + 3 + 1\n"
                         "4 * 2 + 1\n"
                         "4 / 2 * 2 + 1\n"
                         "4 + 2 * 1 * 2 / 3 - 1\n"
                         "-4 - -2 * 3\n"
                         "true == !0\n"
                         "false != true\n"
                         "2 >= 2\n"
                         "3 > 2\n"
                         "1 <= 2\n"
                         "1 + 2 < 2 + 4\n"
                         "(1 + 2) * 3\n"
                         "i = 0\n"
                         "if(x == y && x == z || y == z) { let z = 0 }\n"
                         "if(x == y || x == z && y == z) { z = 0 } else { z = 1 }\n"
                         "fun(x, y, z) { x + y * z }\n"
                         "testFunc(2)\n"
                         "testFunc(x, y)\n"
                         "4 % 3 + 1\n";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(!parser.hasError());
    REQUIRE(printer.print(*programAst.getNodes()[0]) == "((4 + 3) + 1)");
    REQUIRE(printer.print(*programAst.getNodes()[1]) == "((4 * 2) + 1)");
    REQUIRE(printer.print(*programAst.getNodes()[2]) == "(((4 / 2) * 2) + 1)");
    REQUIRE(printer.print(*programAst.getNodes()[3]) == "((4 + (((2 * 1) * 2) / 3)) - 1)");
    REQUIRE(printer.print(*programAst.getNodes()[4]) == "((-4) - (-(2 * 3)))");
    REQUIRE(printer.print(*programAst.getNodes()[5]) == "(True == (!0))");
    REQUIRE(printer.print(*programAst.getNodes()[6]) == "(False != True)");
    REQUIRE(printer.print(*programAst.getNodes()[7]) == "(2 >= 2)");
    REQUIRE(printer.print(*programAst.getNodes()[8]) == "(3 > 2)");
    REQUIRE(printer.print(*programAst.getNodes()[9]) == "(1 <= 2)");
    REQUIRE(printer.print(*programAst.getNodes()[10]) == "((1 + 2) < (2 + 4))");
    REQUIRE(printer.print(*programAst.getNodes()[11]) == "((1 + 2) * 3)");
    REQUIRE(printer.print(*programAst.getNodes()[12]) == "i = 0");
    REQUIRE(printer.print(*programAst.getNodes()[13]) == "if(((x == y) && (x == z)) || (y == z)){\n\tlet z = 0\n}");
    REQUIRE(printer.print(*programAst.getNodes()[14]) == "if((x == y) || ((x == z) && (y == z))){\n\tz = 0\n} else{\n\tz = 1\n}");
    REQUIRE(printer.print(*programAst.getNodes()[15]) == "fun(x, y, z){\n\t(x + (y * z))\n}");
    REQUIRE(printer.print(*programAst.getNodes()[16]) == "testFunc(2)");
    REQUIRE(printer.print(*programAst.getNodes()[17]) == "testFunc(x, y)");
    REQUIRE(printer.print(*programAst.getNodes()[18]) == "((4 % 3) + 1)");
}

TEST_CASE("Parser parses let statements succesfully", "[parse]")
{
    std::string script = "let k = \"YEAH\"\n"
                         "let i = 5\n"
                         "let j = 5.1\n"
                         "let t = 1 * 2 + 3 < 10";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(printer.print(*programAst.getNodes()[0]) == "let k = \"YEAH\"");
    REQUIRE(printer.print(*programAst.getNodes()[1]) == "let i = 5");
    REQUIRE(printer.print(*programAst.getNodes()[2]) == "let j = 5.1");
    REQUIRE(printer.print(*programAst.getNodes()[3]) == "let t = (((1 * 2) + 3) < 10)");
}

TEST_CASE("Parser parses complete script succesfully", "[parse]")
{
    std::string script = "let h=0\n"
                         "let s=0\n"
                         "let v=0\n"
                         "\n"
                         "let update = fun(delta) {\n"
                         "\tif(vol == 0 || freq == 0 || vol < lVol) {\n"
                         "\t\tif(v > 0.05) { v = v - 0.05 }\n"
                         "\t\telse { v = 0 }\n"
                         "\t} else {\n"
                         "\t\th = map(freq, 0, maxF, 180, 0)\n"
                         "\t\ts = 1\n"
                         "\t\tv = map(vol, 0, 100, 0, 1)\n"
                         "\t}\n"
                         "\tlet color = xhsv(h, s, v)\n"
                         "\tsetAllLeds(color)\n"
                         "}\n"
                         "\n"
                         "let init = fun(){}\n"
                         "let scriptName = \"simpleScript\"";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);
}

TEST_CASE("Parser parses array declaration succesfully", "[parse]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(printer.print(*programAst.getNodes()[0]) == "let testArray = [1, \"lorem\", fun(){\n"
                                                        "\t(1 + 2)\n"
                                                        "}]");
}

TEST_CASE("Parser parses indexing succesfully", "[parse]")
{
    std::string script = "let testArray = [1, \"lorem\", fun() { 1 + 2 }]\n"
                         "testArray[0] = 2\n"
                         "testArray[0]";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(printer.print(*programAst.getNodes()[1]) == "testArray[0] = 2");
}

TEST_CASE("Parser parses while loops successfully", "[scanTokens]")
{
    std::string script = "let i = 0\n"
                         "while(i < 10) {\n"
                         "i = i + 1\n"
                         "}\n"
                         "i";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(printer.print(*programAst.getNodes()[1]) == "while(i < 10){\n"
                                                        "\ti = (i + 1)\n"
                                                        "}");
}

TEST_CASE("Parser parses for loops successfully", "[scanTokens]")
{
    std::string script = "for(let i = 0; i < 10; i = i + 1){\n"
                         "}";

    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    auto parser = SyncBlink::Parser(source);
    auto programAst = parser.parse();

    REQUIRE(!parser.hasError());
    REQUIRE(programAst.getNodes().size() > 0);

    SyncBlink::ScriptPrinter printer(source);
    REQUIRE(printer.print(*programAst.getNodes()[0]) == "for(let i = 0; (i < 10); i = (i + 1)){\n"
                                                        "}");
}