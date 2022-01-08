#ifndef WHILEEXPRESSION_H
#define WHILEEXPRESSION_H

#include "parser/ast/ast_node.hpp"

namespace SyncBlink
{
    class WhileExpression : public AstNode
    {
    public:
        WhileExpression(std::unique_ptr<const AstNode> condition, std::unique_ptr<const AstNode> loopBody, int line);

        void accept(AstVisitor& visitor) const;
        const AstNode& getCondition() const;
        const AstNode& getLoopBody() const;
        int getLine() const;

    private:        
        int _line;
        std::unique_ptr<const AstNode> _condition;
        std::unique_ptr<const AstNode> _loopBody;
    };
}

#endif // WHILEEXPRESSION_H