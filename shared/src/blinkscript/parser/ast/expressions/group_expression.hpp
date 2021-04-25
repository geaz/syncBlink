#ifndef GROUPEXPRESSION_H
#define GROUPEXPRESSION_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/scanner/model/token.hpp"

namespace SyncBlink
{
    class GroupExpression : public AstNode
    {
    public:
        explicit GroupExpression(std::unique_ptr<const AstNode> expression);
        void accept(AstVisitor& visitor) const;
        const AstNode& getExpression() const;

    private:
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // GROUPEXPRESSION_H