#ifndef FUNCTIONEXPRESSION_H
#define FUNCTIONEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "scanner/model/token.hpp"

#include <vector>

namespace SyncBlink
{
    class FunctionExpression : public AstNode
    {
    public:
        FunctionExpression(const std::vector<Token>& parameters, std::unique_ptr<const AstNode> functionBody, int line);

        void accept(AstVisitor& visitor) const;
        int getLine() const;
        const AstNode& getFunctionBody() const;
        const std::vector<Token>& getParameters() const;

    private:
        int _line;
        std::vector<Token> _parameters;
        std::unique_ptr<const AstNode> _functionBody;
    };
}

#endif // FUNCTIONEXPRESSION_H