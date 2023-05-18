#include "assign_expression.hpp"

namespace SyncBlink
{
    AssignExpression::AssignExpression(const Token& identifier, std::unique_ptr<const AstNode> expression)
        : _identifier(identifier), _expression(std::move(expression))
    {
    }

    void AssignExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitAssignExpression(*this);
    }

    const Token& AssignExpression::getIdentifier() const
    {
        return _identifier;
    }
    
    const AstNode& AssignExpression::getExpression() const
    {
        return *_expression;
    }
}