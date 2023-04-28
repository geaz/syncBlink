#ifndef CONDITIONEXPRESSION_H
#define CONDITIONEXPRESSION_H

#include "parser/ast/ast_node.hpp"

namespace SyncBlink
{
    class ConditionExpression : public AstNode
    {
    public:
        ConditionExpression(std::unique_ptr<const AstNode> condition, std::unique_ptr<const AstNode> ifStatements,
                            std::unique_ptr<const AstNode> elseStatements, int line);

        void accept(AstVisitor& visitor) const;
        const AstNode& getCondition() const;
        const AstNode& getIfBranch() const;
        const std::unique_ptr<const AstNode>& getElseBranch() const;
        int getLine() const;

    private:
        int _line;
        std::unique_ptr<const AstNode> _condition;
        std::unique_ptr<const AstNode> _ifStatements;
        std::unique_ptr<const AstNode> _elseStatements;
    };
}

#endif // CONDITIONEXPRESSION_H