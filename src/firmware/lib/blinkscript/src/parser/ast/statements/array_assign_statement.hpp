#ifndef ARRAYASSIGNSTATEMENT_H
#define ARRAYASSIGNSTATEMENT_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"
#include "parser/ast/expressions/index_expression.hpp"
#include "parser/model/token.hpp"

namespace SyncBlink
{
    class ArrayAssignStatement : public AstNode
    {
    public:
        ArrayAssignStatement(std::unique_ptr<const AstNode> indexExpression, std::unique_ptr<const AstNode> expression, uint16_t line);
        void accept(AstVisitor& visitor) const;

        uint16_t getLine() const;
        const AstNode& getIndex() const;
        const AstNode* getIndexPtr() const;
        const AstNode& getExpression() const;

    private:
        uint16_t _line;
        std::unique_ptr<const AstNode> _indexExpression;
        std::unique_ptr<const AstNode> _expression;
    };
}

#endif // ARRAYASSIGNSTATEMENT_H