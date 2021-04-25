#ifndef INFIXEXPRESSION_H
#define INFIXEXPRESSION_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/scanner/model/token.hpp"

namespace SyncBlink
{
    class InfixExpression : public AstNode
    {
    public:
        InfixExpression(const Token& operatorToken, std::unique_ptr<const AstNode> left,
                        std::unique_ptr<const AstNode> right);

        void accept(AstVisitor& visitor) const;

        const Token& getOperatorToken() const;
        const AstNode& getLeft() const;
        const AstNode& getRight() const;

    private:
        Token _operatorToken;
        std::unique_ptr<const AstNode> _left;
        std::unique_ptr<const AstNode> _right;
    };
}

#endif // INFIXEXPRESSION_H