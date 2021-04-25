#ifndef SCRIPTPRINTER_H
#define SCRIPTPRINTER_H

#include "blinkscript/parser/ast/ast_node.hpp"
#include "blinkscript/parser/ast/ast_visitor.hpp"
#include "blinkscript/parser/ast/expressions/array_expression.hpp"
#include "blinkscript/parser/ast/expressions/call_expression.hpp"
#include "blinkscript/parser/ast/expressions/condition_expression.hpp"
#include "blinkscript/parser/ast/expressions/for_expression.hpp"
#include "blinkscript/parser/ast/expressions/function_expression.hpp"
#include "blinkscript/parser/ast/expressions/group_expression.hpp"
#include "blinkscript/parser/ast/expressions/index_expression.hpp"
#include "blinkscript/parser/ast/expressions/infix_expression.hpp"
#include "blinkscript/parser/ast/expressions/literal_expression.hpp"
#include "blinkscript/parser/ast/expressions/prefix_expression.hpp"
#include "blinkscript/parser/ast/expressions/while_expression.hpp"
#include "blinkscript/parser/ast/statements/array_assign_statement.hpp"
#include "blinkscript/parser/ast/statements/assign_statement.hpp"
#include "blinkscript/parser/ast/statements/block_statement.hpp"
#include "blinkscript/parser/ast/statements/expression_statement.hpp"
#include "blinkscript/parser/ast/statements/let_statement.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SyncBlink
{
    class ScriptPrinter : public AstVisitor
    {
    public:
        std::string print(const AstNode& node);

        void visitLetStatement(const LetStatement& letStatement) override;
        void visitBlockStatement(const BlockStatement& blockStatement) override;
        void visitAssignStatement(const AssignStatement& assignStatement) override;
        void visitArrayAssignStatement(const ArrayAssignStatement& arrayAssignStatement) override;
        void visitExpressionStatement(const ExpressionStatement& expressionStatement) override;

        void visitInfixExpression(const InfixExpression& infixExpr) override;
        void visitPrefixExpression(const PrefixExpression& prefixExpr) override;
        void visitGroupExpression(const GroupExpression& groupExpr) override;
        void visitConditionExpression(const ConditionExpression& conditionExpr) override;
        void visitFunctionExpression(const FunctionExpression& functionExpr) override;
        void visitCallExpression(const CallExpression& callExpr) override;
        void visitWhileExpression(const WhileExpression& whileExpr) override;
        void visitForExpression(const ForExpression& forExpr) override;
        void visitArrayExpression(const ArrayExpression& arrayExpr) override;
        void visitIndexExpression(const IndexExpression& indexExpr) override;
        void visitLiteralExpression(const LiteralExpression& literalExpr) override;

    private:
        std::string _lastVisit;
    };
}

#endif // SCRIPTPRINTER_H