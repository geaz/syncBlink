#ifndef PREFIXEXPRESSION_H
#define PREFIXEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class PrefixExpression : public AstNode
    {
    public:
        PrefixExpression(const Token& operatorToken, std::unique_ptr<const AstNode> expression);

        void accept(AstVisitor& visitor) const;

        const Token& getOperatorToken() const;
        const AstNode& getExpression() const;

    private:
        Token _operatorToken;
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // PREFIXEXPRESSION_H