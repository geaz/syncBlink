#ifndef FOREXPRESSION_H
#define FOREXPRESSION_H

#include "parser/ast/ast_node.hpp"

#include <vector>

namespace SyncBlink
{
    class ForExpression : public AstNode
    {
    public:
        ForExpression(std::unique_ptr<const AstNode> assignStatement, std::unique_ptr<const AstNode> conditionExpression,
                      std::unique_ptr<const AstNode> incrementorStatement, std::unique_ptr<const AstNode> loopBody, size_t line);

        void accept(AstVisitor& visitor) const;
        const AstNode& getAssignStatement() const;
        const AstNode& getConditionExpression() const;
        const AstNode& getIncrementorStatement() const;
        const AstNode& getLoopBody() const;
        size_t getLine() const;

    private:
        std::unique_ptr<const AstNode> _assignStatement;
        std::unique_ptr<const AstNode> _conditionExpression;
        std::unique_ptr<const AstNode> _incrementorStatement;
        std::unique_ptr<const AstNode> _loopBody;
        size_t _line;
    };
}

#endif // FOREXPRESSION_H