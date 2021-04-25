#ifndef BLOCKSTATEMENT_H
#define BLOCKSTATEMENT_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/parser/ast/ast_visitor.hpp"

#include <memory>
#include <vector>

namespace SyncBlink
{
    class BlockStatement : public AstNode
    {
    public:
        explicit BlockStatement(std::vector<std::unique_ptr<const AstNode>>&& statements);
        void accept(AstVisitor& visitor) const;

        const std::vector<std::unique_ptr<const AstNode>>& getStatements() const;

    private:
        std::vector<std::unique_ptr<const AstNode>> _statements;
    };
}

#endif // BLOCKSTATEMENT_H