#ifndef BLOCKEXPRESSION_H
#define BLOCKEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class BlockExpression : public AstNode
    {
    public:
        explicit BlockExpression(std::vector<std::unique_ptr<const AstNode>>&& statements);
        void accept(AstVisitor& visitor) const;

        const std::vector<std::unique_ptr<const AstNode>>& getStatements() const;

    private:
        std::vector<std::unique_ptr<const AstNode>> _statements;
    };
}

#endif // BLOCKEXPRESSION_H