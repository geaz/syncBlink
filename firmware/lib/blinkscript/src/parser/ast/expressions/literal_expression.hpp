#ifndef LITERALEXPRESSION_H
#define LITERALEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "scanner/model/token.hpp"

namespace SyncBlink
{
    class LiteralExpression : public AstNode
    {
    public:
        explicit LiteralExpression(const Token& valueToken);
        void accept(AstVisitor& visitor) const;

        const Token& getValueToken() const;

    private:
        Token _valueToken;
    };
}

#endif // LITERALEXPRESSION_H