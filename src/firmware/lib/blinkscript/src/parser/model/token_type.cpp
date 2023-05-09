#include "token_type.hpp"

namespace SyncBlink
{
    std::map<TokenType, int> TokenPrecedences = {{TokenType::OR, 1},          {TokenType::AND, 2},        {TokenType::EQUAL, 3},
                                                 {TokenType::EQUAL_EQUAL, 3}, {TokenType::BANG_EQUAL, 3}, {TokenType::LESS, 3},
                                                 {TokenType::GREATER, 3},     {TokenType::LESS_EQUAL, 3}, {TokenType::GREATER_EQUAL, 3},
                                                 {TokenType::PLUS, 4},        {TokenType::MINUS, 4},      {TokenType::STAR, 5},
                                                 {TokenType::SLASH, 5},       {TokenType::LEFT_PAREN, 6}, {TokenType::LEFT_BRACKET, 7}};

    std::map<std::string, TokenType> TokenKeywords = {{"if", TokenType::IF},       {"else", TokenType::ELSE}, {"true", TokenType::TRUE},
                                                      {"false", TokenType::FALSE}, {"let", TokenType::LET},   {"fun", TokenType::FUN},
                                                      {"while", TokenType::WHILE}, {"for", TokenType::FOR}};
}