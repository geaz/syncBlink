#include "function_expression.hpp"

namespace SyncBlink
{
    FunctionExpression::FunctionExpression(const std::vector<Token>& parameters,
                                           std::unique_ptr<const AstNode> functionBody, uint16_t line)
        : _parameters(parameters), _functionBody(std::move(functionBody)), _line(line)
    {
    }

    void FunctionExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitFunctionExpression(*this);
    }
    
    uint16_t FunctionExpression::getLine() const
    {
        return _line;
    }

    const AstNode& FunctionExpression::getFunctionBody() const
    {
        return *_functionBody;
    }

    const std::vector<Token>& FunctionExpression::getParameters() const
    {
        return _parameters;
    }
}