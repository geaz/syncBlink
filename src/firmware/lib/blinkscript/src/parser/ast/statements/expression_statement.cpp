#include "expression_statement.hpp"

namespace SyncBlink
{
    ExpressionStatement::ExpressionStatement(std::unique_ptr<const AstNode> expression) : _expression(std::move(expression))
    {
    }
    void ExpressionStatement::accept(AstVisitor& visitor) const
    {
        visitor.visitExpressionStatement(*this);
    }
    const AstNode& ExpressionStatement::getExpression() const
    {
        return *_expression;
    }
}