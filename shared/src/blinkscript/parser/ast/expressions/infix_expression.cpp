#include "infix_expression.hpp"

namespace SyncBlink
{
    InfixExpression::InfixExpression(const Token& operatorToken, std::unique_ptr<const AstNode> left,
                                     std::unique_ptr<const AstNode> right)
        : _operatorToken(operatorToken), _left(std::move(left)), _right(std::move(right))
    {
    }

    void InfixExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitInfixExpression(*this);
    }
    const Token& InfixExpression::getOperatorToken() const
    {
        return _operatorToken;
    }
    const AstNode& InfixExpression::getLeft() const
    {
        return *_left;
    }
    const AstNode& InfixExpression::getRight() const
    {
        return *_right;
    }
}