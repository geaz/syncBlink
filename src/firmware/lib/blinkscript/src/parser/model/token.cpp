#include "token.hpp"

#include <stdlib.h>

namespace SyncBlink
{
    Token::Token()
    {
    }

    Token::Token(TokenType type, size_t start, size_t length, uint32_t line)
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

    float Token::getNumber(std::shared_ptr<ScriptSource> source) const
    {
        float returnValue = 0;
        if (_type == TokenType::NUMBER)
        {
            returnValue = (float)atof(getLexem(source).c_str());
        }
        return returnValue;
    }

    std::string Token::getString(std::shared_ptr<ScriptSource> source) const
    {
        std::string returnValue = "";
        if (_type == TokenType::STRING)
        {
            returnValue = source->substr(_start + 1, _length - 2);
        }
        return returnValue;
    }

    std::string Token::getLexem(std::shared_ptr<ScriptSource> source) const
    {
        return source->substr(_start, _length);
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