#ifndef ARRAYEXPRESSION_H
#define ARRAYEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "scanner/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class ArrayExpression : public AstNode
    {
    public:
        explicit ArrayExpression(std::vector<std::unique_ptr<const AstNode>>&& arrayContent, uint16_t line);

        void accept(AstVisitor& visitor) const;
        uint16_t getLine() const;
        const std::vector<std::unique_ptr<const AstNode>>& getArrayContent() const;

    private:
        uint16_t _line;
        std::vector<std::unique_ptr<const AstNode>> _arrayContent;
    };
}

#endif // ARRAYEXPRESSION_H