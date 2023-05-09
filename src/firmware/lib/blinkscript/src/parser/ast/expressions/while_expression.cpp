#include "while_expression.hpp"

namespace SyncBlink
{
    WhileExpression::WhileExpression(std::unique_ptr<const AstNode> condition, std::unique_ptr<const AstNode> loopBody, uint16_t line)
        : _condition(std::move(condition)), _loopBody(std::move(loopBody)), _line(line)
    {
    }

    void WhileExpression::accept(AstVisitor& visitor) const
    {
        visitor.visitWhileExpression(*this);
    }

    const AstNode& WhileExpression::getCondition() const
    {
        return *_condition;
    }

    const AstNode& WhileExpression::getLoopBody() const
    {
        return *_loopBody;
    }

    uint16_t WhileExpression::getLine() const
    {
        return _line;
    }
}