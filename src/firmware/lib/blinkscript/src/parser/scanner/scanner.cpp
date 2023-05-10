#include "scanner.hpp"

namespace SyncBlink
{
    Scanner::Scanner(std::shared_ptr<ScriptSource> source) : _source(source)
    {
    }

    Token Scanner::advance()
    {
        return scanToken(true);
    }

    Token Scanner::peek()
    {
        return scanToken(false);
    }

    Token Scanner::peek(int skip)
    {
        return scanToken(false, false, TokenType::FALSE, skip);
    }

    Token Scanner::getCurrent() const
    {
        return _currentToken;
    }

    bool Scanner::isEnd() const
    {
        return _currentPos >= _source->length();
    }

    bool Scanner::peekAdvance(TokenType match)
    {
        return scanToken(false, true, match).getTokenType() == match;
    }

    Token Scanner::scanToken(bool advance, bool advanceMatch, TokenType match, uint32_t skip)
    {
        Token token;
        if (_currentPos >= _source->length() || (skip > 0 && _currentPos + skip + 1 >= _source->length()))
            return Token(TokenType::ENDOFFILE, 0, 0, _line);

        skipNonTokens();
        uint32_t originalStart = _currentPos;

        for (uint32_t i = 0; i < skip; i++)
        {
            _currentPos++;
            skipNonTokens();
        }
        _startPos = _currentPos++;

        char currentChar = _source->getChar(_startPos);
        switch (currentChar)
        {
        case '\n':
            token = Token(TokenType::EOL, _startPos, 0, _line++);
            break;
        case '(':
            token = Token(TokenType::LEFT_PAREN, _startPos, 1, _line);
            break;
        case ')':
            token = Token(TokenType::RIGHT_PAREN, _startPos, 1, _line);
            break;
        case '{':
            token = Token(TokenType::LEFT_BRACE, _startPos, 1, _line);
            break;
        case '}':
            token = Token(TokenType::RIGHT_BRACE, _startPos, 1, _line);
            break;
        case '[':
            token = Token(TokenType::LEFT_BRACKET, _startPos, 1, _line);
            break;
        case ']':
            token = Token(TokenType::RIGHT_BRACKET, _startPos, 1, _line);
            break;
        case ',':
            token = Token(TokenType::COMMA, _startPos, 1, _line);
            break;
        case ';':
            token = Token(TokenType::SEMICOLON, _startPos, 1, _line);
            break;
        case '.':
            token = Token(TokenType::DOT, _startPos, 1, _line);
            break;
        case '-':
            token = Token(TokenType::MINUS, _startPos, 1, _line);
            break;
        case '+':
            token = Token(TokenType::PLUS, _startPos, 1, _line);
            break;
        case '/':
            token = Token(TokenType::SLASH, _startPos, 1, _line);
            break;
        case '*':
            token = Token(TokenType::STAR, _startPos, 1, _line);
            break;
        case '&':
            if (peekAdvance('&'))
            {
                token = Token(TokenType::AND, _startPos, 2, _line);
                _currentPos++;
            }
            break;
        case '|':
            if (peekAdvance('|'))
            {
                token = Token(TokenType::OR, _startPos, 2, _line);
                _currentPos++;
            }
            break;
        case '!':
            if (peekAdvance('='))
            {
                token = Token(TokenType::BANG_EQUAL, _startPos, 2, _line);
                _currentPos++;
            }
            else
                token = Token(TokenType::BANG, _startPos, 1, _line);
            break;
        case '=':
            if (peekAdvance('='))
            {
                token = Token(TokenType::EQUAL_EQUAL, _startPos, 2, _line);
                _currentPos++;
            }
            else
                token = Token(TokenType::EQUAL, _startPos, 1, _line);
            break;
        case '<':
            if (peekAdvance('='))
            {
                token = Token(TokenType::LESS_EQUAL, _startPos, 2, _line);
                _currentPos++;
            }
            else
                token = Token(TokenType::LESS, _startPos, 1, _line);
            break;
        case '>':
            if (peekAdvance('='))
            {
                token = Token(TokenType::GREATER_EQUAL, _startPos, 2, _line);
                _currentPos++;
            }
            else
                token = Token(TokenType::GREATER, _startPos, 1, _line);
            break;
        case '"':
            token = consumeString();
            break;
        default:
            if (isDigit(currentChar)) token = consumeNumber();
            else if (isAlpha(currentChar))
                token = consumeIdentifier();
            break;
        }

        // Reset Line and CurrentPos, if we just peeked or token was not matching
        if (!advance && !(advanceMatch && token.getTokenType() == match))
        {
            _currentPos = originalStart;
            _line -= token.getTokenType() == TokenType::EOL ? 1 : 0;
        }
        // Save new token
        else
            _currentToken = token;

        return token;
    }

    void Scanner::skipNonTokens()
    {
        bool skipped = true;
        while (skipped && _currentPos < _source->length())
        {
            switch (_source->getChar(_currentPos))
            {
            case '/':
                if (_source->getChar(_currentPos + 1) == '/') consumeComment();
                else
                    skipped = false;
                break;
            case ' ':
            case '\r':
            case '\t':
                _currentPos++;
                break;
            default:
                skipped = false;
                break;
            }
        }
    }

    void Scanner::consumeComment()
    {
        while (_currentPos < _source->length() && _source->getChar(_currentPos) != '\n')
            _currentPos++;
    }

    Token Scanner::consumeString()
    {
        Token stringToken;
        while (_currentPos < _source->length() && _source->getChar(_currentPos) != '\n' && _source->getChar(_currentPos) != '"')
            _currentPos++;
        if (_currentPos < _source->length())
        {
            _currentPos++; // Consume the closing "
            stringToken = Token(TokenType::STRING, _startPos, _currentPos - _startPos, _line);
        }
        return stringToken;
    }

    Token Scanner::consumeNumber()
    {
        _currentPos--; // go back one position
        while (_currentPos < _source->length() && isDigit(_source->getChar(_currentPos)))
            _currentPos++;
        if (_currentPos < _source->length() && _source->getChar(_currentPos) == '.' && _currentPos + 1 < _source->length() &&
            isDigit(_source->getChar(_currentPos + 1)))
        {
            _currentPos++;
            while (_currentPos < _source->length() && isDigit(_source->getChar(_currentPos)))
                _currentPos++;
        }
        return Token(TokenType::NUMBER, _startPos, _currentPos - _startPos, _line);
    }

    Token Scanner::consumeIdentifier()
    {
        _currentPos--; // go back one position
        while (_currentPos < _source->length() && (isAlpha(_source->getChar(_currentPos)) || isDigit(_source->getChar(_currentPos))))
            _currentPos++;

        TokenType type = TokenType::IDENTIFIER;
        std::string lexem = _source->substr(_startPos, _currentPos - _startPos);

        auto iterator = TokenKeywords.find(lexem);
        if (iterator != TokenKeywords.end()) type = iterator->second;

        return Token(type, _startPos, _currentPos - _startPos, _line);
    }

    bool Scanner::peekAdvance(char match)
    {
        bool result = false;
        if (_currentPos < _source->length() && _source->getChar(_currentPos) == match)
        {
            _currentPos++;
            result = true;
        }
        return result;
    }

    bool Scanner::isDigit(const char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool Scanner::isAlpha(const char c) const
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }
}