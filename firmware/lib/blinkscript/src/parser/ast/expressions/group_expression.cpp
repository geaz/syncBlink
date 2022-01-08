#include "group_expression.hpp"

namespace SyncBlink
{
    GroupExpression::GroupExpression(std::unique_ptr<const AstNode> expression) : _expression(std::move(expression))
    {
    }

    void GroupExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitGroupExpression(*this);
    }
    const AstNode& GroupExpression::getExpression() const
    {
        return *_expression;
    }
}