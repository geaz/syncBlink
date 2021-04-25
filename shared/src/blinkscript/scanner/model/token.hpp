#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.hpp"

#include <map>
#include <string>
#include <vector>

namespace SyncBlink
{
    class Token
    {
    public:
        Token();
        Token(const TokenType type, const char* start, const uint16_t length, const uint16_t line);

        int getLine() const;
        int getPrecedence() const;
        TokenType getTokenType() const;

        float getNumber() const;
        std::string getString() const;
        std::string getLexem() const;

    private:
        TokenType _type = TokenType::EMPTY;
        const char* _start;
        uint16_t _length;
        uint16_t _line;
    };
}

#endif // TOKEN_H