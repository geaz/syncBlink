#ifndef ASSIGNEXPRESSION_H
#define ASSIGNEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class AssignExpression : public AstNode
    {
    public:
        AssignExpression(const Token& identifier, std::unique_ptr<const AstNode> expression);
        void accept(AstVisitor& visitor) const;

        const Token& getIdentifier() const;
        const AstNode& getExpression() const;

    private:
        Token _identifier;
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // ASSIGNEXPRESSION_H