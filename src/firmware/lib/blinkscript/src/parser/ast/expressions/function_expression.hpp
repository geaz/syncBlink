#ifndef FUNCTIONEXPRESSION_H
#define FUNCTIONEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class FunctionExpression : public AstNode
    {
    public:
        FunctionExpression(const std::vector<Token>& parameters, std::unique_ptr<const AstNode> functionBody, size_t line);

        void accept(AstVisitor& visitor) const;
        size_t getLine() const;
        const AstNode& getFunctionBody() const;
        const AstNode* getFunctionBodyPtr() const;
        const std::vector<Token>& getParameters() const;

    private:
        std::vector<Token> _parameters;
        std::unique_ptr<const AstNode> _functionBody;
        size_t _line;
    };
}

#endif // FUNCTIONEXPRESSION_H