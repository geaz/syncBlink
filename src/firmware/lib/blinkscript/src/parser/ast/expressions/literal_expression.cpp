#include "literal_expression.hpp"

#include <sstream>

namespace SyncBlink
{
    LiteralExpression::LiteralExpression(const Token& valueToken) : _valueToken(valueToken)
    {
    }
    
    void LiteralExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitLiteralExpression(*this);
    }

    const Token& LiteralExpression::getValueToken() const
    {
        return _valueToken;
    }
}