#ifndef SCANNER_H
#define SCANNER_H

#include "model/token.hpp"

#include <string>
#include <vector>

namespace SyncBlink
{
    class Scanner
    {
    public:
        explicit Scanner(const std::string& source);

        bool peekAdvance(TokenType match);
        Token advance();
        Token peek();
        Token peek(int skip);
        Token getCurrent() const;
        bool isEnd() const;

    private:
        Token scanToken(bool advance, bool advanceMatch = false, TokenType match = TokenType::FALSE, uint32_t skip = 0);

        void skipNonTokens();
        void consumeComment();
        Token consumeString();
        Token consumeNumber();
        Token consumeIdentifier();

        bool peekAdvance(char match);
        bool isDigit(const char c) const;
        bool isAlpha(const char c) const;

        std::string _source;
        Token _currentToken;
        uint32_t _currentPos = 0;
        uint32_t _startPos = 0;
        uint32_t _line = 1;
    };
}

#endif // SCANNER_H