#ifndef SCRIPTPRINTER_H
#define SCRIPTPRINTER_H

#include "parser/ast/ast_node.hpp"
#include "parser/ast/ast_visitor.hpp"
#include "parser/ast/expressions/block_expression.hpp"
#include "parser/ast/expressions/array_expression.hpp"
#include "parser/ast/expressions/call_expression.hpp"
#include "parser/ast/expressions/condition_expression.hpp"
#include "parser/ast/expressions/while_expression.hpp"
#include "parser/ast/expressions/for_expression.hpp"
#include "parser/ast/expressions/function_expression.hpp"
#include "parser/ast/expressions/group_expression.hpp"
#include "parser/ast/expressions/index_expression.hpp"
#include "parser/ast/expressions/infix_expression.hpp"
#include "parser/ast/expressions/literal_expression.hpp"
#include "parser/ast/expressions/prefix_expression.hpp"
#include "parser/ast/expressions/array_assign_expression.hpp"
#include "parser/ast/expressions/assign_expression.hpp"
#include "parser/ast/statements/expression_statement.hpp"
#include "parser/ast/statements/let_statement.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    class ScriptPrinter : public AstVisitor
    {
    public:
        ScriptPrinter(std::shared_ptr<ScriptSource> source);

        std::string print(const AstNode& node);

        void visitLetStatement(const LetStatement& letStatement) override;
        void visitExpressionStatement(const ExpressionStatement& expressionStatement) override;

        void visitBlockExpression(const BlockExpression& blockExpression) override;
        void visitAssignExpression(const AssignExpression& assignExpression) override;
        void visitArrayAssignExpression(const ArrayAssignExpression& arrayAssignExpression) override;
        void visitInfixExpression(const InfixExpression& infixExpr) override;
        void visitPrefixExpression(const PrefixExpression& prefixExpr) override;
        void visitGroupExpression(const GroupExpression& groupExpr) override;
        void visitConditionExpression(const ConditionExpression& conditionExpr) override;
        void visitWhileExpression(const WhileExpression& whileExpression) override;
        void visitForExpression(const ForExpression& forExpression) override;
        void visitFunctionExpression(const FunctionExpression& functionExpr) override;
        void visitCallExpression(const CallExpression& callExpr) override;
        void visitArrayExpression(const ArrayExpression& arrayExpr) override;
        void visitIndexExpression(const IndexExpression& indexExpr) override;
        void visitLiteralExpression(const LiteralExpression& literalExpr) override;

    private:
        std::string _lastVisit;
        std::shared_ptr<ScriptSource> _source;
    };
}

#endif // SCRIPTPRINTER_H