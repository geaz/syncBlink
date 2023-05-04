#ifndef WHILEEXPRESSION_H
#define WHILEEXPRESSION_H

#include "parser/ast/ast_node.hpp"

namespace SyncBlink
{
    class WhileExpression : public AstNode
    {
    public:
        WhileExpression(std::unique_ptr<const AstNode> condition, std::unique_ptr<const AstNode> loopBody, uint16_t line);

        void accept(AstVisitor& visitor) const;
        const AstNode& getCondition() const;
        const AstNode& getLoopBody() const;
        uint16_t getLine() const;

    private:
        std::unique_ptr<const AstNode> _condition;
        std::unique_ptr<const AstNode> _loopBody;
        uint16_t _line;
    };
}

#endif // WHILEEXPRESSION_H