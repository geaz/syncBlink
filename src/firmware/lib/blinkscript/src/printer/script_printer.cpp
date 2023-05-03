#include "script_printer.hpp"

namespace SyncBlink
{
    ScriptPrinter::ScriptPrinter(std::shared_ptr<ScriptSource> source) : _source(source) { }

    std::string ScriptPrinter::print(const AstNode& node)
    {
        node.accept(*this);
        return _lastVisit;
    }

    void ScriptPrinter::visitLetStatement(const LetStatement& letStatement)
    {
        _lastVisit = "let " + letStatement.getIdentifier().getLexem(_source) + " = " + print(letStatement.getExpression());
    }

    void ScriptPrinter::visitBlockStatement(const BlockStatement& blockStatement)
    {
        std::string blockPrint;
        for (auto& statement : blockStatement.getStatements())
        {
            blockPrint += '\t' + print(*statement) + "\n";
        }
        _lastVisit = "{\n" + blockPrint + "}";
    }

    void ScriptPrinter::visitAssignStatement(const AssignStatement& assignStatement)
    {
        _lastVisit = assignStatement.getIdentifier().getLexem(_source) + " = " + print(assignStatement.getExpression());
    }

    void ScriptPrinter::visitArrayAssignStatement(const ArrayAssignStatement& arrayAssignStatement)
    {
        _lastVisit = print(arrayAssignStatement.getIndex()) + " = " + print(arrayAssignStatement.getExpression());
    }

    void ScriptPrinter::visitExpressionStatement(const ExpressionStatement& expressionStatement)
    {
        _lastVisit = print(expressionStatement.getExpression());
    }

    void ScriptPrinter::visitInfixExpression(const InfixExpression& infixExpr)
    {
        _lastVisit = "(" + print(infixExpr.getLeft()) + " " + infixExpr.getOperatorToken().getLexem(_source) + " " +
                     print(infixExpr.getRight()) + ")";
    }

    void ScriptPrinter::visitPrefixExpression(const PrefixExpression& prefixExpr)
    {
        _lastVisit = "(" + prefixExpr.getOperatorToken().getLexem(_source) + print(prefixExpr.getExpression()) + ")";
    }

    void ScriptPrinter::visitGroupExpression(const GroupExpression& groupExpr)
    {
        _lastVisit = print(groupExpr.getExpression());
    }

    void ScriptPrinter::visitConditionExpression(const ConditionExpression& conditionExpr)
    {
        std::string ifBranch = "if" + print(conditionExpr.getCondition()) + print(conditionExpr.getIfBranch());
        std::string elseBranch;
        if (conditionExpr.getElseBranch() != nullptr)
        {
            elseBranch = " else" + print(*conditionExpr.getElseBranch());
        }
        _lastVisit = ifBranch + elseBranch;
    }

    void ScriptPrinter::visitFunctionExpression(const FunctionExpression& functionExpr)
    {
        std::string funPrint = "fun(";
        auto parameters = functionExpr.getParameters();
        for (int i = 0; i < parameters.size(); i++)
        {
            funPrint += parameters[i].getLexem(_source);
            if (i < parameters.size() - 1)
                funPrint += ", ";
        }
        _lastVisit = funPrint + ")" + print(functionExpr.getFunctionBody());
    }

    void ScriptPrinter::visitCallExpression(const CallExpression& callExpr)
    {
        std::string callPrint = callExpr.getIdentifier().getLexem(_source) + "(";
        auto& parameters = callExpr.getParameters();
        for (int i = 0; i < parameters.size(); i++)
        {
            callPrint += print(*parameters[i]);
            if (i < parameters.size() - 1)
                callPrint += ", ";
        }
        _lastVisit = callPrint + ")";
    }

    void ScriptPrinter::visitWhileExpression(const WhileExpression& whileExpr)
    {
        _lastVisit = "while" + print(whileExpr.getCondition()) + print(whileExpr.getLoopBody());
    }

    void ScriptPrinter::visitForExpression(const ForExpression& forExpr)
    {
        _lastVisit = "for(" + print(forExpr.getAssignStatement()) + "; " + print(forExpr.getConditionExpression()) +
                     "; " + print(forExpr.getIncrementorStatement()) + ")" + print(forExpr.getLoopBody());
    }

    void ScriptPrinter::visitArrayExpression(const ArrayExpression& arrayExpr)
    {
        std::string arrayPrint = "[";
        auto& content = arrayExpr.getArrayContent();
        for (int i = 0; i < content.size(); i++)
        {
            arrayPrint += print(*content[i]);
            if (i < content.size() - 1)
                arrayPrint += ", ";
        }
        _lastVisit = arrayPrint + "]";
    }

    void ScriptPrinter::visitIndexExpression(const IndexExpression& indexExpr)
    {
        _lastVisit = print(indexExpr.getLeft()) + "[" + print(indexExpr.getRight()) + "]";
    }

    void ScriptPrinter::visitLiteralExpression(const LiteralExpression& literalExpr)
    {
        std::string value = "";
        Token valueToken = literalExpr.getValueToken();
        if (valueToken.getTokenType() == TokenType::IDENTIFIER || valueToken.getTokenType() == TokenType::STRING ||
            valueToken.getTokenType() == TokenType::NUMBER)
            value = valueToken.getLexem(_source);
        else if (valueToken.getTokenType() == TokenType::FALSE)
            value = "False";
        else if (valueToken.getTokenType() == TokenType::TRUE)
            value = "True";
        _lastVisit = value;
    }
}