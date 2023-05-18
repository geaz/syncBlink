#include "call_expression.hpp"

namespace SyncBlink
{
    CallExpression::CallExpression(const Token& identifier, std::vector<std::unique_ptr<const AstNode>>&& parameters)
        : _identifier(identifier), _parameters(std::move(parameters))
    {
    }

    void CallExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitCallExpression(*this);
    }

    const Token& CallExpression::getIdentifier() const
    {
        return _identifier;
    }

    const std::vector<std::unique_ptr<const AstNode>>& CallExpression::getParameters() const
    {
        return _parameters;
    }
}