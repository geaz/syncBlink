#include "parser.hpp"

#include "ast/expressions/array_expression.hpp"
#include "ast/expressions/call_expression.hpp"
#include "ast/expressions/condition_expression.hpp"
#include "ast/expressions/for_expression.hpp"
#include "ast/expressions/while_expression.hpp"
#include "ast/expressions/function_expression.hpp"
#include "ast/expressions/group_expression.hpp"
#include "ast/expressions/index_expression.hpp"
#include "ast/expressions/infix_expression.hpp"
#include "ast/expressions/literal_expression.hpp"
#include "ast/expressions/prefix_expression.hpp"
#include "ast/expressions/array_assign_expression.hpp"
#include "ast/expressions/assign_expression.hpp"
#include "ast/expressions/block_expression.hpp"
#include "ast/statements/expression_statement.hpp"
#include "ast/statements/let_statement.hpp"

#include <algorithm>

namespace SyncBlink
{
    Parser::Parser(std::shared_ptr<ScriptSource> source) : _source(source), _scanner(source)
    {
        _nud[TokenType::STRING] = &Parser::parseLiteralExpression;
        _nud[TokenType::NUMBER] = &Parser::parseLiteralExpression;
        _nud[TokenType::TRUE] = &Parser::parseLiteralExpression;
        _nud[TokenType::FALSE] = &Parser::parseLiteralExpression;
        _nud[TokenType::IDENTIFIER] = &Parser::parseLiteralExpression;
        _nud[TokenType::MINUS] = &Parser::parsePrefixExpression;
        _nud[TokenType::BANG] = &Parser::parsePrefixExpression;
        _nud[TokenType::LEFT_PAREN] = &Parser::parseGroupExpression;
        _nud[TokenType::IF] = &Parser::parseConditionExpression;
        _nud[TokenType::FUN] = &Parser::parseFunctionExpression;
        _nud[TokenType::LEFT_BRACKET] = &Parser::parseArrayExpression;
        _nud[TokenType::WHILE] = &Parser::parseWhileExpression;
        _nud[TokenType::FOR] = &Parser::parseForExpression;

        _led[TokenType::EQUAL] = &Parser::parseAssignExpression;
        _led[TokenType::LEFT_PAREN] = &Parser::parseCallExpression;
        _led[TokenType::PLUS] = &Parser::parseInfixExpression;
        _led[TokenType::MINUS] = &Parser::parseInfixExpression;
        _led[TokenType::STAR] = &Parser::parseInfixExpression;
        _led[TokenType::SLASH] = &Parser::parseInfixExpression;
        _led[TokenType::MODULO] = &Parser::parseInfixExpression;
        _led[TokenType::AND] = &Parser::parseInfixExpression;
        _led[TokenType::OR] = &Parser::parseInfixExpression;
        _led[TokenType::EQUAL_EQUAL] = &Parser::parseInfixExpression;
        _led[TokenType::BANG_EQUAL] = &Parser::parseInfixExpression;
        _led[TokenType::GREATER_EQUAL] = &Parser::parseInfixExpression;
        _led[TokenType::GREATER] = &Parser::parseInfixExpression;
        _led[TokenType::LESS_EQUAL] = &Parser::parseInfixExpression;
        _led[TokenType::LESS] = &Parser::parseInfixExpression;
        _led[TokenType::LEFT_BRACKET] = &Parser::parseIndexExpression;
    }

    ProgramAst Parser::parse()
    {
        ProgramAst programAst(_source);
        while (_scanner.advance().getTokenType() != TokenType::ENDOFFILE)
        {
            std::unique_ptr<const AstNode> statement = parseStatement();
            if (statement != nullptr) programAst.addNode(std::move(statement));
        }
        return std::move(programAst);
    }

    bool Parser::hasError() const
    {
        return std::get<1>(_parserError) != "";
    }

    std::tuple<size_t, std::string> Parser::getError() const
    {
        return _parserError;
    }

    std::unique_ptr<const AstNode> Parser::parseStatement()
    {
        std::unique_ptr<const AstNode> statement = nullptr;
        Token currentToken = _scanner.getCurrent();
        switch (currentToken.getTokenType())
        {
        case TokenType::EOL:
            break;
        case TokenType::LET:
            statement = parseLetStatement();
            break;
        default: {
            // We need to check, if there is a equal sign on the current line
            // Otherwise we don't know, if it is an arrayassignstatement
            bool equalOnLine = _scanner.peekLine(TokenType::EQUAL);
            if (_scanner.peek().getTokenType() == TokenType::LEFT_BRACKET && equalOnLine)
                statement = parseArrayAssignExpression();
            else
                statement = parseExpressionStatement();
            break;
        }
        }
        return statement;
    }

    std::unique_ptr<const AstNode> Parser::parseLetStatement()
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::LET}) && assertToken(_scanner.advance(), {TokenType::IDENTIFIER}))
        {
            Token identifier(_scanner.getCurrent());
            if (assertToken(_scanner.advance(), {TokenType::EQUAL}))
            {
                _scanner.advance();
                std::unique_ptr<const AstNode> expression = parseExpression(0);
                if (expression != nullptr) return std::unique_ptr<LetStatement>(new LetStatement(identifier, std::move(expression)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseBlockExpression()
    {
        std::unique_ptr<const AstNode> blockStatement = nullptr;
        std::vector<std::unique_ptr<const AstNode>> statements;
        _scanner.peekAdvance(TokenType::EOL); // consume optional EOL before {
        if (assertToken(_scanner.advance(), {TokenType::LEFT_BRACE}))
        {
            while (!_scanner.isEnd() && _scanner.peek().getTokenType() != TokenType::RIGHT_BRACE)
            {
                _scanner.advance();
                std::unique_ptr<const AstNode> statement = parseStatement();
                if (statement != nullptr) statements.push_back(std::move(statement));
            }
            if (assertToken(_scanner.advance(), {TokenType::RIGHT_BRACE}))
            {
                blockStatement = std::unique_ptr<BlockExpression>(new BlockExpression(std::move(statements)));
                _scanner.peekAdvance(TokenType::EOL); // consume optional EOL after }
            }
        }
        return blockStatement;
    }

    std::unique_ptr<const AstNode> Parser::parseArrayAssignExpression()
    {
        auto indexExpression = parseExpression(5);
        auto currentToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::RIGHT_BRACKET}) && assertToken(_scanner.advance(), {TokenType::EQUAL}))
        {
            _scanner.advance();
            auto expression = parseExpression(0);
            if (expression != nullptr)
            {
                auto arrayAssignExpression = std::unique_ptr<ArrayAssignExpression>(
                    new ArrayAssignExpression(std::move(indexExpression), std::move(expression), currentToken.getLine()));
                return std::unique_ptr<ExpressionStatement>(new ExpressionStatement(std::move(arrayAssignExpression)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseExpressionStatement()
    {
        std::unique_ptr<const AstNode> expression = parseExpression(0);
        if (expression != nullptr) return std::unique_ptr<ExpressionStatement>(new ExpressionStatement(std::move(expression)));
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseExpression(size_t precedence)
    {
        std::unique_ptr<const AstNode> expression = nullptr;
        Token currentToken = _scanner.getCurrent();
        auto iter = _nud.find(currentToken.getTokenType());
        if (iter == _nud.end())
        {
            _parserError = std::tuple<size_t, std::string>(currentToken.getLine(), "Expected NUD!");
        }
        else
        {
            expression = (this->*(iter->second))();
            while (_scanner.peek().getTokenType() != TokenType::ENDOFFILE && _scanner.peek().getTokenType() != TokenType::EOL &&
                   precedence < _scanner.peek().getPrecedence())
            {
                Token lastToken = _scanner.getCurrent();
                currentToken = _scanner.advance();

                auto iter = _led.find(currentToken.getTokenType());
                if (iter == _led.end())
                {
                    _parserError = std::tuple<size_t, std::string>(currentToken.getLine(), "Expected LED!");
                }
                else
                {
                    expression = (this->*(iter->second))(std::move(expression), lastToken);
                }
            }
        }
        return expression;
    }

    std::unique_ptr<const AstNode> Parser::parseLiteralExpression()
    {
        if (assertToken(_scanner.getCurrent(),
                        {TokenType::STRING, TokenType::NUMBER, TokenType::TRUE, TokenType::FALSE, TokenType::IDENTIFIER}))
        {
            return std::unique_ptr<LiteralExpression>(new LiteralExpression(_scanner.getCurrent()));                
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parsePrefixExpression()
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::MINUS, TokenType::BANG}))
        {
            Token oper = _scanner.getCurrent();
            size_t precedence = _scanner.getCurrent().getPrecedence();
            _scanner.advance();

            auto expression = parseExpression(precedence);
            if (expression != nullptr)
            {
                return std::unique_ptr<PrefixExpression>(new PrefixExpression(oper, std::move(expression)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseGroupExpression()
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::LEFT_PAREN}))
        {
            _scanner.advance();
            auto expression = parseExpression(0);
            if (expression != nullptr && assertToken(_scanner.advance(), {TokenType::RIGHT_PAREN}))
            {
                return std::unique_ptr<GroupExpression>(new GroupExpression(std::move(expression)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseConditionExpression()
    {
        auto ifToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::IF}) && assertToken(_scanner.advance(), {TokenType::LEFT_PAREN}))
        {
            std::unique_ptr<const AstNode> condition = parseGroupExpression();
            std::unique_ptr<const AstNode> ifStatements = parseBlockExpression();
            std::unique_ptr<const AstNode> elseStatements = nullptr;

            if (_scanner.peekAdvance(TokenType::ELSE))
            {
                elseStatements = parseBlockExpression();
            }

            if (condition != nullptr && ifStatements != nullptr)
            {
                return std::unique_ptr<ConditionExpression>(
                    new ConditionExpression(std::move(condition), std::move(ifStatements), std::move(elseStatements), ifToken.getLine()));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseWhileExpression()
    {
        auto whileToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::WHILE}) && assertToken(_scanner.advance(), {TokenType::LEFT_PAREN}))
        {
            auto condition = parseGroupExpression();
            if (assertToken(_scanner.peek(), {TokenType::LEFT_BRACE}))
            {
                std::unique_ptr<const AstNode> blockExpression = parseBlockExpression();
                if (blockExpression != nullptr)
                    return std::unique_ptr<WhileExpression>(
                        new WhileExpression(std::move(condition), std::move(blockExpression), whileToken.getLine()));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseForExpression()
    {
        auto forToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::FOR}) && assertToken(_scanner.advance(), {TokenType::LEFT_PAREN}))
        {
            _scanner.advance(); // consume LEFT_PAREN
            auto assignStatement = parseStatement();
            assertToken(_scanner.advance(), {TokenType::SEMICOLON});
            _scanner.advance(); // consume SEMICOLON
            auto conditionExpression = parseExpression(0);
            assertToken(_scanner.advance(), {TokenType::SEMICOLON});
            _scanner.advance(); // consume SEMICOLON
            auto incrementorStatement = parseStatement();
            assertToken(_scanner.advance(), {TokenType::RIGHT_PAREN});

            if (assignStatement == nullptr || conditionExpression == nullptr || incrementorStatement == nullptr)
            {
                _parserError = std::tuple<size_t, std::string>(_scanner.getCurrent().getLine(), "Missing loop parameter!");
            }
            else if (assertToken(_scanner.peek(), {TokenType::LEFT_BRACE}))
            {
                std::unique_ptr<const AstNode> blockExpression = parseBlockExpression();
                if (blockExpression != nullptr)
                    return std::unique_ptr<ForExpression>(new ForExpression(std::move(assignStatement), std::move(conditionExpression),
                                                                            std::move(incrementorStatement), std::move(blockExpression),
                                                                            forToken.getLine()));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseFunctionExpression()
    {
        auto funToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::FUN}) && assertToken(_scanner.advance(), {TokenType::LEFT_PAREN}))
        {
            auto parameters = parseFunctionParameters();
            if (assertToken(_scanner.advance(), {TokenType::RIGHT_PAREN}))
            {
                std::unique_ptr<const AstNode> functionBody = parseBlockExpression();
                if (functionBody != nullptr)
                    return std::unique_ptr<FunctionExpression>(
                        new FunctionExpression(parameters, std::move(functionBody), funToken.getLine()));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseArrayExpression()
    {
        auto arrayStartToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::LEFT_BRACKET}))
        {
            auto arrayContent = parseExpressionList(TokenType::RIGHT_BRACKET);
            if (assertToken(_scanner.advance(), {TokenType::RIGHT_BRACKET}))
            {
                return std::unique_ptr<ArrayExpression>(new ArrayExpression(std::move(arrayContent), arrayStartToken.getLine()));
            }            
        }
        return nullptr;
    }    

    std::unique_ptr<const AstNode> Parser::parseAssignExpression(std::unique_ptr<const AstNode> left, Token lastToken)
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::EQUAL}))
        {
            _scanner.advance();
            std::unique_ptr<const AstNode> expression = parseExpression(0);
            if (expression != nullptr) return std::unique_ptr<AssignExpression>(new AssignExpression(lastToken, std::move(expression)));
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseCallExpression(std::unique_ptr<const AstNode> left, Token lastToken)
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::LEFT_PAREN}))
        {
            auto parameters = parseExpressionList(TokenType::RIGHT_PAREN);
            if (assertToken(_scanner.advance(), {TokenType::RIGHT_PAREN}))
            {
                return std::unique_ptr<CallExpression>(new CallExpression(lastToken, std::move(parameters)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseInfixExpression(std::unique_ptr<const AstNode> left, Token lastToken)
    {
        if (assertToken(_scanner.getCurrent(), {TokenType::PLUS, TokenType::MINUS, TokenType::STAR, TokenType::SLASH, TokenType::MODULO,
                                                TokenType::OR, TokenType::AND, TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL,
                                                TokenType::GREATER_EQUAL, TokenType::GREATER, TokenType::LESS_EQUAL, TokenType::LESS}))
        {
            Token oper = _scanner.getCurrent();
            size_t precedence = _scanner.getCurrent().getPrecedence();
            _scanner.advance();

            std::unique_ptr<const AstNode> right = parseExpression(precedence);
            if (left != nullptr && right != nullptr)
            {
                return std::unique_ptr<InfixExpression>(new InfixExpression(oper, std::move(left), std::move(right)));
            }
        }
        return nullptr;
    }

    std::unique_ptr<const AstNode> Parser::parseIndexExpression(std::unique_ptr<const AstNode> left, Token lastToken)
    {
        Token startToken = _scanner.getCurrent();
        if (assertToken(_scanner.getCurrent(), {TokenType::LEFT_BRACKET}))
        {
            _scanner.advance(); // consume '[' we want to parse the index and not an array expression
            std::unique_ptr<const AstNode> index = parseExpression(0);
            if (assertToken(_scanner.advance(), {TokenType::RIGHT_BRACKET}))
            {
                return std::unique_ptr<IndexExpression>(new IndexExpression(std::move(left), std::move(index), startToken.getLine()));
            }
        }
        return nullptr;
    }

    std::vector<Token> Parser::parseFunctionParameters()
    {
        bool parameterParseSuccess = true;
        std::vector<Token> parameters;
        while (!_scanner.isEnd() && parameterParseSuccess && _scanner.peek().getTokenType() != TokenType::RIGHT_PAREN)
        {
            if (assertToken(_scanner.advance(), {TokenType::IDENTIFIER}))
            {
                parameters.push_back(_scanner.getCurrent());
                parameterParseSuccess =
                    _scanner.peek().getTokenType() == TokenType::RIGHT_PAREN || assertToken(_scanner.advance(), {TokenType::COMMA});
            }
        }
        return parameters;
    }

    std::vector<std::unique_ptr<const AstNode>> Parser::parseExpressionList(TokenType endToken)
    {
        bool expressionParseSuccess = true;
        std::vector<std::unique_ptr<const AstNode>> expressions;
        while (!_scanner.isEnd() && expressionParseSuccess && _scanner.peek().getTokenType() != endToken)
        {
            _scanner.advance();
            std::unique_ptr<const AstNode> expression = parseExpression(0);
            expressionParseSuccess = expression != nullptr;

            if (expressionParseSuccess)
            {
                expressions.push_back(std::move(expression));
                expressionParseSuccess = _scanner.peek().getTokenType() == endToken || assertToken(_scanner.advance(), {TokenType::COMMA});
            }
        }
        return expressions;
    }

    bool Parser::assertToken(Token token, std::vector<TokenType> expected)
    {
        bool result = std::find(expected.begin(), expected.end(), token.getTokenType()) != expected.end();
        if (!result) _parserError = std::tuple<size_t, std::string>(token.getLine(), "Unexpected Token!");
        return result;
    }
}