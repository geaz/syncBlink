#include "block_expression.hpp"

namespace SyncBlink
{
    BlockExpression::BlockExpression(std::vector<std::unique_ptr<const AstNode>>&& statements) : _statements(std::move(statements))
    {
    }

    void BlockExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitBlockExpression(*this);
    }

    const std::vector<std::unique_ptr<const AstNode>>& BlockExpression::getStatements() const
    {
        return _statements;
    }
}