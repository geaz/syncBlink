#include "array_expression.hpp"

namespace SyncBlink
{
    ArrayExpression::ArrayExpression(std::vector<std::unique_ptr<const AstNode>>&& arrayContent, size_t line)
        : _arrayContent(std::move(arrayContent)), _line(line)
    {
    }

    void ArrayExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitArrayExpression(*this);
    }

    size_t ArrayExpression::getLine() const
    {
        return _line;
    }

    const std::vector<std::unique_ptr<const AstNode>>& ArrayExpression::getArrayContent() const
    {
        return _arrayContent;
    }
}