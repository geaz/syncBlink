#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <map>
#include <string>
#include <vector>

namespace SyncBlink
{
    enum class TokenType
    {
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        SEMICOLON,
        DOT,
        MINUS,
        PLUS,
        SLASH,
        STAR,
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,
        AND,
        OR,
        IDENTIFIER,
        STRING,
        NUMBER,
        ELSE,
        FALSE,
        IF,
        TRUE,
        LET,
        FUN,
        WHILE,
        FOR,
        EOL,
        ENDOFFILE,
        EMPTY
    };

    extern std::map<TokenType, int> TokenPrecedences;
    extern std::map<std::string, TokenType> TokenKeywords;
}

#endif // TOKENTYPE_H