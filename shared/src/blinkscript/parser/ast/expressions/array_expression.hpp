#ifndef ARRAYEXPRESSION_H
#define ARRAYEXPRESSION_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/scanner/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class ArrayExpression : public AstNode
    {
    public:
        explicit ArrayExpression(std::vector<std::unique_ptr<const AstNode>>&& arrayContent, int line);

        void accept(AstVisitor& visitor) const;
        int getLine() const;
        const std::vector<std::unique_ptr<const AstNode>>& getArrayContent() const;

    private:
        int _line;
        std::vector<std::unique_ptr<const AstNode>> _arrayContent;
    };
}

#endif // ARRAYEXPRESSION_H