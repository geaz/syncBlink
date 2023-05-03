#include <catch2/catch.hpp>
#include "scanner/scanner.hpp"
#include "scanner/model/token.hpp"
#include "scanner/string_script_source.hpp"
#include <iostream>

TEST_CASE("Scanner reports error on unrecognized lexems", "[scanTokens]")
{
    std::string script = "##&%";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);

    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() == SyncBlink::TokenType::EMPTY);
    }
}

TEST_CASE("Scanner is able to scan first non-sense script", "[scanTokens]")
{
    std::string script = "// this is a comment\n(( )){} // grouping stuff\n!*+-/=<> <= == // operators\n|| &&";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);

    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() != SyncBlink::TokenType::EMPTY);
    }
}

TEST_CASE("Scanner is able to scan string", "[scanTokens]")
{
    std::string script = "\"TestString\"";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    SyncBlink::Token token;

    REQUIRE(scanner.advance().getString(source) == "TestString");
}

TEST_CASE("Scanner is able to scan numbers", "[scanTokens]")
{
    std::string script = "13\n456\n4.78";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    SyncBlink::Token token;

    REQUIRE(scanner.advance().getNumber(source) == 13.0f);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getNumber(source) == 456.0f);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getNumber(source) == 4.78f);
}

TEST_CASE("Scanner is able to scan identifiers", "[scanTokens]")
{
    std::string script = "let letTest = 2\n"
                         "let funTest = fun(){ }";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    SyncBlink::Token token;

    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::LET);
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::IDENTIFIER);
    REQUIRE(scanner.getCurrent().getLexem(source) == "letTest");
}

TEST_CASE("Scanner is able to scan keywords", "[scanTokens]")
{
    std::string script = "true\nfalse\nif\nelse\nlet";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    SyncBlink::Token token;
    
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::TRUE);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::FALSE);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::IF);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::ELSE);
    scanner.advance(); // Skip EOL
    REQUIRE(scanner.advance().getTokenType() == SyncBlink::TokenType::LET);
}

TEST_CASE("Scanner is able to scan arrays", "[scanTokens]")
{
    std::string script = "let testArray = []\n"
                         "testArray[0] = 1\n"
                         "testArray[1] = \"lorem\"\n"
                         "testArray[2] = fun() {}";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    SyncBlink::Token token;
    
    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() != SyncBlink::TokenType::EMPTY);
    }
}

TEST_CASE("Scanner is able to scan script", "[scanTokens]")
{
    std::string script =    "let h=0\n"
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
    SyncBlink::Scanner scanner(source);
    
    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() != SyncBlink::TokenType::EMPTY);
    }
}

TEST_CASE("Scanner is able to scan while loops", "[scanTokens]")
{
    std::string script = "let i = 0\n"
                         "while(i < 10) {\n"
                         "i = i + 1\n"
                         "}\n"
                         "i";
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    
    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() != SyncBlink::TokenType::EMPTY);
    }
}

TEST_CASE("Scanner is able to scan for loops", "[scanTokens]")
{
    std::string script = "for(let i = 0; i < 10; i = i + 1){\n"
                         "}";
    
    auto source = std::make_shared<SyncBlink::StringScriptSource>(script);
    SyncBlink::Scanner scanner(source);
    
    while(scanner.advance().getTokenType() != SyncBlink::TokenType::ENDOFFILE)
    {
        REQUIRE(scanner.getCurrent().getTokenType() != SyncBlink::TokenType::EMPTY);
    }
}