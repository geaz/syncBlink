#ifndef PARSER_H
#define PARSER_H

#include "ast/ast_node.hpp"
#include "ast/program_ast.hpp"
#include "model/token.hpp"
#include "model/token_type.hpp"
#include "scanner/scanner.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace SyncBlink
{
    class Parser;
    typedef std::unique_ptr<const AstNode> (Parser::*NudParserFunc)();
    typedef std::unique_ptr<const AstNode> (Parser::*LedParserFunc)(std::unique_ptr<const AstNode>, Token lastToken);

    class Parser
    {
    public:
        explicit Parser(std::shared_ptr<ScriptSource> source);
        ProgramAst parse();

        bool hasError() const;
        std::tuple<int, std::string> getError() const;

    private:
        std::unique_ptr<const AstNode> parseStatement();
        std::unique_ptr<const AstNode> parseLetStatement();
        std::unique_ptr<const AstNode> parseBlockStatement();
        std::unique_ptr<const AstNode> parseAssignStatement();
        std::unique_ptr<const AstNode> parseArrayAssignStatement();
        std::unique_ptr<const AstNode> parseExpressionStatement();

        std::unique_ptr<const AstNode> parseExpression(int precedence);
        std::unique_ptr<const AstNode> parseLiteralExpression();
        std::unique_ptr<const AstNode> parsePrefixExpression();
        std::unique_ptr<const AstNode> parseGroupExpression();
        std::unique_ptr<const AstNode> parseConditionExpression();
        std::unique_ptr<const AstNode> parseFunctionExpression();
        std::unique_ptr<const AstNode> parseWhileExpression();
        std::unique_ptr<const AstNode> parseForExpression();
        std::unique_ptr<const AstNode> parseArrayExpression();

        std::unique_ptr<const AstNode> parseCallExpression(std::unique_ptr<const AstNode> left, Token lastToken);
        std::unique_ptr<const AstNode> parseInfixExpression(std::unique_ptr<const AstNode> left, Token lastToken);
        std::unique_ptr<const AstNode> parseIndexExpression(std::unique_ptr<const AstNode> left, Token lastToken);

        std::vector<Token> parseFunctionParameters();
        std::vector<std::unique_ptr<const AstNode>> parseExpressionList(TokenType endToken);
        bool assertToken(Token token, std::vector<TokenType> expected);

        std::shared_ptr<ScriptSource> _source;
        Scanner _scanner;
        std::tuple<int, std::string> _parserError = std::make_tuple(-99, "");

        std::map<TokenType, NudParserFunc> _nud;
        std::map<TokenType, LedParserFunc> _led;
    };
}

#endif // PARSER_H