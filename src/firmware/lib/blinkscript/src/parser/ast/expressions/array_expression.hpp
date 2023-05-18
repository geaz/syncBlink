#ifndef ARRAYEXPRESSION_H
#define ARRAYEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class ArrayExpression : public AstNode
    {
    public:
        explicit ArrayExpression(std::vector<std::unique_ptr<const AstNode>>&& arrayContent, size_t line);

        void accept(AstVisitor& visitor) const;
        size_t getLine() const;
        const std::vector<std::unique_ptr<const AstNode>>& getArrayContent() const;

    private:
        std::vector<std::unique_ptr<const AstNode>> _arrayContent;
        size_t _line;
    };
}

#endif // ARRAYEXPRESSION_H