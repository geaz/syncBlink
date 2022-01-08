#ifndef CALLEXPRESSION_H
#define CALLEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "scanner/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class CallExpression : public AstNode
    {
    public:
        CallExpression(const Token& identifier, std::vector<std::unique_ptr<const AstNode>>&& parameters);

        void accept(AstVisitor& visitor) const;
        const Token& getIdentifier() const;
        const std::vector<std::unique_ptr<const AstNode>>& getParameters() const;

    private:
        Token _identifier;
        std::vector<std::unique_ptr<const AstNode>> _parameters;
    };
}

#endif // CALLEXPRESSION_H