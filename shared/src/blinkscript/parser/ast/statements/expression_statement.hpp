#ifndef EXPRESSIONSTATEMENT_H
#define EXPRESSIONSTATEMENT_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/scanner/model/token.hpp"

namespace SyncBlink
{
    class ExpressionStatement : public AstNode
    {
    public:
        explicit ExpressionStatement(std::unique_ptr<const AstNode> expression);
        void accept(AstVisitor& visitor) const;

        const AstNode& getExpression() const;

    private:
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // EXPRESSIONSTATEMENT_H