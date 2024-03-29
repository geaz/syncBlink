#ifndef INDEXEXPRESSION_H
#define INDEXEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class IndexExpression : public AstNode
    {
    public:
        IndexExpression(std::unique_ptr<const AstNode> left, std::unique_ptr<const AstNode> right, size_t line);

        void accept(AstVisitor& visitor) const;

        size_t getLine() const;
        const AstNode& getLeft() const;
        const AstNode& getRight() const;

    private:
        std::unique_ptr<const AstNode> _left;
        std::unique_ptr<const AstNode> _right;
        size_t _line;
    };
}

#endif // INDEXEXPRESSION_H