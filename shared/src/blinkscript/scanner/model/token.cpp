#include "token.hpp"

#include <stdlib.h>

namespace SyncBlink
{
    Token::Token()
    {
    }

    Token::Token(const TokenType type, const char* start, const uint16_t length, const uint16_t line)
        : _type(type), _start(start), _length(length), _line(line)
    {
    }

    int Token::getPrecedence() const
    {
        int precedence = 0;
        auto iter = TokenPrecedences.find(getTokenType());
        if (iter != TokenPrecedences.end())
            precedence = iter->second;
        return precedence;
    }

    float Token::getNumber() const
    {
        float returnValue = 0;
        if (_type == TokenType::NUMBER)
        {
            returnValue = (float)atof(getLexem().c_str());
        }
        return returnValue;
    }

    std::string Token::getString() const
    {
        std::string returnValue = "";
        if (_type == TokenType::STRING)
        {
            returnValue = std::string(_start + 1, _length - 2);
        }
        return returnValue;
    }

    std::string Token::getLexem() const
    {
        return std::string(_start, _length);
    }

    int Token::getLine() const
    {
        return _line;
    }

    TokenType Token::getTokenType() const
    {
        return _type;
    }
}