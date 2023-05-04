#include "prefix_expression.hpp"

namespace SyncBlink
{
    PrefixExpression::PrefixExpression(const Token& operatorToken, std::unique_ptr<const AstNode> expression)
        : _operatorToken(operatorToken), _expression(std::move(expression))
    {
    }

    void PrefixExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitPrefixExpression(*this);
    }

    const Token& PrefixExpression::getOperatorToken() const
    {
        return _operatorToken;
    }
    
    const AstNode& PrefixExpression::getExpression() const
    {
        return *_expression;
    }
}