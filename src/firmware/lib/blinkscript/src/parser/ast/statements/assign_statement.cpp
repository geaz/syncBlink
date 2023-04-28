#include "assign_statement.hpp"

namespace SyncBlink
{
    AssignStatement::AssignStatement(const Token& identifier, std::unique_ptr<const AstNode> expression)
        : _identifier(identifier), _expression(std::move(expression))
    {
    }

    void AssignStatement::accept(AstVisitor& visitor) const
    {
        visitor.visitAssignStatement(*this);
    }
    const Token& AssignStatement::getIdentifier() const
    {
        return _identifier;
    }
    const AstNode& AssignStatement::getExpression() const
    {
        return *_expression;
    }
}