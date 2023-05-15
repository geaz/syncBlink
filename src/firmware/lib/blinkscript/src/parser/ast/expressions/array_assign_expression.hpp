#ifndef ARRAYASSIGNEXPRESSION_H
#define ARRAYASSIGNEXPRESSION_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"
#include "parser/ast/expressions/index_expression.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class ArrayAssignExpression : public AstNode
    {
    public:
        ArrayAssignExpression(std::unique_ptr<const AstNode> indexExpression, std::unique_ptr<const AstNode> expression, size_t line);
        void accept(AstVisitor& visitor) const;

        size_t getLine() const;
        const AstNode& getIndex() const;
        const AstNode* getIndexPtr() const;
        const AstNode& getExpression() const;

    private:
        std::unique_ptr<const AstNode> _indexExpression;
        std::unique_ptr<const AstNode> _expression;
        size_t _line;
    };
}

#endif // ARRAYASSIGNEXPRESSION_H