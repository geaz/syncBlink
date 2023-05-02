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
        FunctionExpression(const std::vector<Token>& parameters, std::unique_ptr<const AstNode> functionBody, uint16_t line);

        void accept(AstVisitor& visitor) const;
        uint16_t getLine() const;
        const AstNode& getFunctionBody() const;
        const std::vector<Token>& getParameters() const;

    private:
        uint16_t _line;
        std::vector<Token> _parameters;
        std::unique_ptr<const AstNode> _functionBody;
    };
}

#endif // FUNCTIONEXPRESSION_H