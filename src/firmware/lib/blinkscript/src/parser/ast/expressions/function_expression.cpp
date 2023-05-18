#include "function_expression.hpp"

namespace SyncBlink
{
    FunctionExpression::FunctionExpression(const std::vector<Token>& parameters, std::unique_ptr<const AstNode> functionBody, size_t line)
        : _parameters(parameters), _functionBody(std::move(functionBody)), _line(line)
    {
    }

    void FunctionExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitFunctionExpression(*this);
    }

    size_t FunctionExpression::getLine() const
    {
        return _line;
    }

    const AstNode& FunctionExpression::getFunctionBody() const
    {
        return *_functionBody;
    }

    const AstNode* FunctionExpression::getFunctionBodyPtr() const
    {
        return _functionBody.get();
    }

    const std::vector<Token>& FunctionExpression::getParameters() const
    {
        return _parameters;
    }
}