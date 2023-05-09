#include "index_expression.hpp"

namespace SyncBlink
{
    IndexExpression::IndexExpression(std::unique_ptr<const AstNode> left, std::unique_ptr<const AstNode> right, uint16_t line)
        : _left(std::move(left)), _right(std::move(right)), _line(line)
    {
    }

    void IndexExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitIndexExpression(*this);
    }

    uint16_t IndexExpression::getLine() const
    {
        return _line;
    }

    const AstNode& IndexExpression::getLeft() const
    {
        return *_left;
    }

    const AstNode& IndexExpression::getRight() const
    {
        return *_right;
    }
}