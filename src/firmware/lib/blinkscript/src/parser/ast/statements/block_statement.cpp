#include "block_statement.hpp"

namespace SyncBlink
{
    BlockStatement::BlockStatement(std::vector<std::unique_ptr<const AstNode>>&& statements)
        : _statements(std::move(statements))
    {
    }

    void BlockStatement::accept(AstVisitor& visitor) const
    {
        visitor.visitBlockStatement(*this);
    }
    const std::vector<std::unique_ptr<const AstNode>>& BlockStatement::getStatements() const
    {
        return _statements;
    }
}