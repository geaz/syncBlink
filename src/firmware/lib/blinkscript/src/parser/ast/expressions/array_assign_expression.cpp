#include "array_assign_expression.hpp"

namespace SyncBlink
{
    ArrayAssignExpression::ArrayAssignExpression(std::unique_ptr<const AstNode> indexExpression, std::unique_ptr<const AstNode> expression,
                                               size_t line)
        : _indexExpression(std::move(indexExpression)), _expression(std::move(expression)), _line(line)
    {
    }

    void ArrayAssignExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitArrayAssignExpression(*this);
    }

    size_t ArrayAssignExpression::getLine() const
    {
        return _line;
    }

    const AstNode& ArrayAssignExpression::getIndex() const
    {
        return *_indexExpression;
    }

    const AstNode* ArrayAssignExpression::getIndexPtr() const
    {
        return _indexExpression.get();
    }

    const AstNode& ArrayAssignExpression::getExpression() const
    {
        return *_expression;
    }
}