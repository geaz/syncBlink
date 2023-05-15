#include "condition_expression.hpp"

namespace SyncBlink
{
    ConditionExpression::ConditionExpression(std::unique_ptr<const AstNode> condition, std::unique_ptr<const AstNode> ifStatements,
                                             std::unique_ptr<const AstNode> elseStatements, size_t line)
        : _condition(std::move(condition)), _ifStatements(std::move(ifStatements)), _elseStatements(std::move(elseStatements)), _line(line)
    {
    }

    void ConditionExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitConditionExpression(*this);
    }

    const AstNode& ConditionExpression::getCondition() const
    {
        return *_condition;
    }

    const AstNode& ConditionExpression::getIfBranch() const
    {
        return *_ifStatements;
    }

    const std::unique_ptr<const AstNode>& ConditionExpression::getElseBranch() const
    {
        return _elseStatements;
    }

    size_t ConditionExpression::getLine() const
    {
        return _line;
    }
}