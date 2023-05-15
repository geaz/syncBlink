#include "let_statement.hpp"

namespace SyncBlink
{
    LetStatement::LetStatement(const Token& identifier, std::unique_ptr<const AstNode> expression)
        : _identifier(identifier), _expression(std::move(expression))
    {
    }

    void LetStatement::accept(AstVisitor& visitor) const
    {
        visitor.visitLetStatement(*this);
    }
    
    const Token& LetStatement::getIdentifier() const
    {
        return _identifier;
    }

    const AstNode& LetStatement::getExpression() const
    {
        return *_expression;
    }
}