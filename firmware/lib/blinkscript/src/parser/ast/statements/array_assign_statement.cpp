#include "array_assign_statement.hpp"

namespace SyncBlink
{
    ArrayAssignStatement::ArrayAssignStatement(std::unique_ptr<const AstNode> indexExpression,
                                               std::unique_ptr<const AstNode> expression, uint32_t line)
        : _indexExpression(std::move(indexExpression)), _expression(std::move(expression)), _line(line)
    {
    }

    void ArrayAssignStatement::accept(AstVisitor& visitor) const
    {
        visitor.visitArrayAssignStatement(*this);
    }

    uint32_t ArrayAssignStatement::getLine() const
    {
        return _line;
    }
    const AstNode& ArrayAssignStatement::getIndex() const
    {
        return *_indexExpression;
    }
    const AstNode* ArrayAssignStatement::getIndexPtr() const
    {
        return _indexExpression.get();
    }
    const AstNode& ArrayAssignStatement::getExpression() const
    {
        return *_expression;
    }
}