#ifndef LETSTATEMENT_H
#define LETSTATEMENT_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class LetStatement : public AstNode
    {
    public:
        LetStatement(const Token& identifier, std::unique_ptr<const AstNode> expression);
        void accept(AstVisitor& visitor) const;

        const Token& getIdentifier() const;
        const AstNode& getExpression() const;

    private:
        Token _identifier;
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // LETSTATEMENT_H