#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.hpp"
#include "source/script_source.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace SyncBlink
{
    class Token
    {
    public:
        Token();
        Token(TokenType type, size_t start, size_t length, uint32_t line);

        int getLine() const;
        int getPrecedence() const;
        TokenType getTokenType() const;

        float getNumber(std::shared_ptr<ScriptSource> source) const;
        std::string getString(std::shared_ptr<ScriptSource> source) const;
        std::string getLexem(std::shared_ptr<ScriptSource> source) const;

    private:
        TokenType _type = TokenType::EMPTY;
        size_t _start = 0;
        size_t _length = 0;
        uint32_t _line = 0;
    };
}

#endif // TOKEN_H