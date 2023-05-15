#ifndef SCANNER_H
#define SCANNER_H

#include "parser/model/token.hpp"
#include "source/script_source.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    class Scanner
    {
    public:
        explicit Scanner(std::shared_ptr<ScriptSource> source);

        Token peek();
        Token advance();
        bool peekAdvance(TokenType match);
        bool peekLine(TokenType match);
        bool peekSequence(std::vector<TokenType> expected);

        Token getCurrent() const;
        bool isEnd() const;

    private:
        Token scanToken(bool advance, bool advanceMatch = false, TokenType match = TokenType::FALSE, size_t skip = 0);

        void skipNonTokens();
        void consumeComment();
        Token consumeString();
        Token consumeNumber();
        Token consumeIdentifier();

        bool peekAdvance(char match);
        bool isDigit(const char c) const;
        bool isAlpha(const char c) const;

        std::shared_ptr<ScriptSource> _source;
        Token _currentToken;
        size_t _currentPos = 0;
        size_t _startPos = 0;
        size_t _line = 1;
    };
}

#endif // SCANNER_H