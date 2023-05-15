#ifndef COMPILER_H
#define COMPILER_H

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
#include "parser/ast/program_ast.hpp"
#include "parser/ast/expressions/array_assign_expression.hpp"
#include "parser/ast/expressions/assign_expression.hpp"
#include "parser/ast/statements/expression_statement.hpp"
#include "parser/ast/statements/let_statement.hpp"
#include "program/model/value.hpp"
#include "program/model/objects/closure_object.hpp"
#include "program/program.hpp"
#include "local_scope.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace SyncBlink
{   
    class Compiler : public AstVisitor
    {
    public:
        Compiler(std::shared_ptr<ScriptSource> source, const ProgramAst& programAst);

        Program compile();

        bool hasError() const;
        std::tuple<size_t, std::string> getError() const;

        void visitExpressionStatement(const ExpressionStatement& expressionStatement) override;
        void visitLetStatement(const LetStatement& letStatement) override;

        void visitGroupExpression(const GroupExpression& groupExpr) override;
        void visitBlockExpression(const BlockExpression& blockExpression) override;
        void visitAssignExpression(const AssignExpression& assignExpression) override;
        void visitArrayAssignExpression(const ArrayAssignExpression& arrayAssignExpression) override;
        void visitInfixExpression(const InfixExpression& infixExpr) override;
        void visitPrefixExpression(const PrefixExpression& prefixExpr) override;
        void visitConditionExpression(const ConditionExpression& conditionExpr) override;
        void visitWhileExpression(const WhileExpression& whileExpression) override;
        void visitForExpression(const ForExpression& forExpression) override;
        void visitFunctionExpression(const FunctionExpression& functionExpr) override;
        void visitCallExpression(const CallExpression& callExpr) override;
        void visitArrayExpression(const ArrayExpression& arrayExpr) override;
        void visitIndexExpression(const IndexExpression& indexExpr) override;
        void visitLiteralExpression(const LiteralExpression& literalExpr) override;

    private:
        void addNumberValueCode(float number, size_t line);
        void addBoolValueCode(bool boolean, size_t line);
        void addArrayValueCode(uint32_t arraySize, size_t line);
        void addStrValueCode(const std::string& string, size_t line);
        void addLoadVariableCode(const std::string& identifier, size_t line);

        std::shared_ptr<LocalScope> removeFrame();

        void checkReturnValue();
        void checkProgramSizes();        

        std::shared_ptr<ScriptSource> _source;
        const ProgramAst& _programAst;
        
        Program _program;
        Program* _targetProgram = &_program;
        std::tuple<size_t, std::string> _error = std::make_tuple(0, "");

        std::vector<std::string> _globalIdentifiers;
        std::shared_ptr<LocalScope> _localScope = std::make_shared<LocalScope>();
    };
}

#endif // COMPILER_H