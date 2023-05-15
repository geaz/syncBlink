#ifndef ASTVISITOR_H
#define ASTVISITOR_H

namespace SyncBlink
{
    class LetStatement;
    class ExpressionStatement;
    class BlockExpression;
    class AssignExpression;
    class ArrayAssignExpression;
    class InfixExpression;
    class PrefixExpression;
    class GroupExpression;
    class ConditionExpression;
    class WhileExpression;
    class ForExpression;
    class FunctionExpression;
    class CallExpression;
    class ArrayExpression;
    class IndexExpression;
    class LiteralExpression;

    class AstVisitor
    {
    public:
        virtual void visitLetStatement(const LetStatement& letStatement) = 0;
        virtual void visitExpressionStatement(const ExpressionStatement& expressionStatement) = 0;

        virtual void visitBlockExpression(const BlockExpression& blockExpression) = 0;
        virtual void visitAssignExpression(const AssignExpression& assignExpression) = 0;
        virtual void visitArrayAssignExpression(const ArrayAssignExpression& arrayAssignExpression) = 0;
        virtual void visitInfixExpression(const InfixExpression& infixExpr) = 0;
        virtual void visitPrefixExpression(const PrefixExpression& prefixExpr) = 0;
        virtual void visitGroupExpression(const GroupExpression& groupExpr) = 0;
        virtual void visitConditionExpression(const ConditionExpression& conditionExpr) = 0;
        virtual void visitWhileExpression(const WhileExpression& whileExpr) = 0;
        virtual void visitForExpression(const ForExpression& forExpr) = 0;
        virtual void visitFunctionExpression(const FunctionExpression& functionExpr) = 0;
        virtual void visitCallExpression(const CallExpression& callExpr) = 0;
        virtual void visitArrayExpression(const ArrayExpression& arrayExpr) = 0;
        virtual void visitIndexExpression(const IndexExpression& indexExpr) = 0;
        virtual void visitLiteralExpression(const LiteralExpression& literalExpr) = 0;
    };
}

#endif // ASTVISITOR_H