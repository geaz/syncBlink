#include "for_expression.hpp"

namespace SyncBlink
{
    ForExpression::ForExpression(std::unique_ptr<const AstNode> assignStatement,
                                 std::unique_ptr<const AstNode> conditionExpression,
                                 std::unique_ptr<const AstNode> incrementorStatement,
                                 std::unique_ptr<const AstNode> loopBody, uint16_t line)
        : _assignStatement(std::move(assignStatement)), _conditionExpression(std::move(conditionExpression)),
          _incrementorStatement(std::move(incrementorStatement)), _loopBody(std::move(loopBody)), _line(line)
    {
    }

    void ForExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitForExpression(*this);
    }

    const AstNode& ForExpression::getAssignStatement() const
    {
        return *_assignStatement;
    }

    const AstNode& ForExpression::getConditionExpression() const
    {
        return *_conditionExpression;
    }

    const AstNode& ForExpression::getIncrementorStatement() const
    {
        return *_incrementorStatement;
    }

    const AstNode& ForExpression::getLoopBody() const
    {
        return *_loopBody;
    }
    
    uint16_t ForExpression::getLine() const
    {
        return _line;
    }
}