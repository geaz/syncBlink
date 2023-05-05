#include "compiler.hpp"

#include "program/model/op_codes.hpp"
#include "program/model/objects/array_object.hpp"
#include "program/model/objects/function_object.hpp"
#include "program/model/objects/string_object.hpp"

#include <algorithm>

namespace SyncBlink
{
    Compiler::Compiler(std::shared_ptr<ScriptSource> source, const ProgramAst& programAst) : _source(source), _programAst(programAst)
    { }

    Program Compiler::compile()
    {
        _error = std::tuple<int, std::string>(-99, "");
        for (auto& node : _programAst.getNodes())
        {
            node->accept(*this);
            checkValueCount();

            if (hasError())
                break;
        }
        return std::move(_program);
    }

    bool Compiler::hasError() const
    {
        return std::get<0>(_error) != -99;
    }

    std::tuple<int, std::string> Compiler::getError() const
    {
        return _error;
    }

    void Compiler::visitExpressionStatement(const ExpressionStatement& expressionStatement)
    {
        expressionStatement.getExpression().accept(*this);
    }

    void Compiler::visitGroupExpression(const GroupExpression& groupExpr)
    {
        groupExpr.getExpression().accept(*this);
    }

    void Compiler::visitBlockStatement(const BlockStatement& blockStatement)
    {
        if (!_skipFraming)
            _targetProgram->addCode(OpCode::FRAME, -1);
        for (auto& node : blockStatement.getStatements())
        {
            node->accept(*this);
        }
        if (!_skipFraming)
            _targetProgram->addCode(OpCode::UNFRAME, -1);
    }

    void Compiler::visitLetStatement(const LetStatement& letStatement)
    {
        letStatement.getExpression().accept(*this);
        _targetProgram->addCode(OpCode::DEFINE, letStatement.getIdentifier().getLine());
        addStrValueCode(letStatement.getIdentifier().getLexem(_source), letStatement.getIdentifier().getLine());
    }

    void Compiler::visitAssignStatement(const AssignStatement& assignStatement)
    {
        assignStatement.getExpression().accept(*this);
        _targetProgram->addCode(OpCode::SET, assignStatement.getIdentifier().getLine());
        addStrValueCode(assignStatement.getIdentifier().getLexem(_source), assignStatement.getIdentifier().getLine());
    }

    void Compiler::visitArrayAssignStatement(const ArrayAssignStatement& arrayAssignStatement)
    {
        arrayAssignStatement.getExpression().accept(*this);
        auto indexExpr = static_cast<const IndexExpression*>(arrayAssignStatement.getIndexPtr());
        indexExpr->getLeft().accept(*this);
        indexExpr->getRight().accept(*this);
        _targetProgram->addCode(OpCode::SET_INDEX, arrayAssignStatement.getLine());
    }

    void Compiler::visitPrefixExpression(const PrefixExpression& prefixExpr)
    {
        prefixExpr.getExpression().accept(*this);
        switch (prefixExpr.getOperatorToken().getTokenType())
        {
        case TokenType::BANG:
            _targetProgram->addCode(OpCode::BANG_NEGATE, prefixExpr.getOperatorToken().getLine());
            break;
        case TokenType::MINUS:
            _targetProgram->addCode(OpCode::NEGATE, prefixExpr.getOperatorToken().getLine());
            break;
        default:
            _error = std::make_tuple(prefixExpr.getOperatorToken().getLine(), "Unexpected Prefix Operator!");
            break;
        }
    }

    void Compiler::visitInfixExpression(const InfixExpression& infixExpr)
    {
        infixExpr.getLeft().accept(*this);
        infixExpr.getRight().accept(*this);
        switch (infixExpr.getOperatorToken().getTokenType())
        {
        case TokenType::PLUS:
            _targetProgram->addCode(OpCode::ADD, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::MINUS:
            _targetProgram->addCode(OpCode::SUB, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::SLASH:
            _targetProgram->addCode(OpCode::DIV, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::STAR:
            _targetProgram->addCode(OpCode::MUL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::AND:
            _targetProgram->addCode(OpCode::AND, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::OR:
            _targetProgram->addCode(OpCode::OR, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::BANG_EQUAL:
            _targetProgram->addCode(OpCode::BANG_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::LESS:
            _targetProgram->addCode(OpCode::LESS, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::LESS_EQUAL:
            _targetProgram->addCode(OpCode::LESS_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::GREATER:
            _targetProgram->addCode(OpCode::GREATER, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::GREATER_EQUAL:
            _targetProgram->addCode(OpCode::GREATER_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::EQUAL_EQUAL:
            _targetProgram->addCode(OpCode::EQUAL_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        default:
            _error = std::make_tuple(infixExpr.getOperatorToken().getLine(), "Unexpected Infix Operator!");
            break;
        }
    }

    void Compiler::visitConditionExpression(const ConditionExpression& conditionExpr)
    {
        conditionExpr.getCondition().accept(*this);

        // get code size of if and else branch to create JUMP code
        auto currentProgram = _targetProgram;

        Program ifProgram ;
        _targetProgram = &ifProgram;
        conditionExpr.getIfBranch().accept(*this);

        bool elsePresent = conditionExpr.getElseBranch() != nullptr;
        Program elseProgram;
        if (elsePresent)
        {           
            _targetProgram = &elseProgram;
            conditionExpr.getElseBranch()->accept(*this); 
        }

        _targetProgram = currentProgram;        
        _targetProgram->addCode(OpCode::JMP_NOT, conditionExpr.getLine());
        _targetProgram->addCode(OpCode::VALUE, conditionExpr.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + ifProgram.getCode().size() + 1.0f + (elsePresent ? 3.0f : 0 /*JMP of If branch, if else present*/),
                                     conditionExpr.getLine());

        conditionExpr.getIfBranch().accept(*this);
        if (elsePresent)
        {
            // JMP beyond else branch, if the if-branch was executed
            _targetProgram->addCode(OpCode::JMP, conditionExpr.getLine());
            _targetProgram->addCode(OpCode::VALUE, conditionExpr.getLine());
            addNumberValueCode(_targetProgram->getCode().size() + elseProgram.getCode().size() + 1.0f, conditionExpr.getLine());

            conditionExpr.getElseBranch()->accept(*this);
        }
    }

    void Compiler::visitFunctionExpression(const FunctionExpression& functionExpr)
    {
        auto currentProgram = _targetProgram;
        Program funProgram;
        _targetProgram = &funProgram;

        functionExpr.getFunctionBody().accept(*this);

        _targetProgram = currentProgram;
        _targetProgram->addCode(OpCode::VALUE, functionExpr.getLine());
        addFunValueCode(std::move(funProgram), functionExpr.getParameters(), functionExpr.getLine());
    }

    void Compiler::visitCallExpression(const CallExpression& callExpr)
    {
        // Push the parameters reversed onto the stack
        for (int i = callExpr.getParameters().size() - 1; i >= 0; i--)
        {
            callExpr.getParameters()[i]->accept(*this);
        }
        _targetProgram->addCode(OpCode::CALL, callExpr.getIdentifier().getLine());
        addStrValueCode(callExpr.getIdentifier().getLexem(_source), callExpr.getIdentifier().getLine());
    }

    void Compiler::visitArrayExpression(const ArrayExpression& arrayExpr)
    {
        for (auto& content : arrayExpr.getArrayContent())
        {
            content->accept(*this);
        }
        // last added values are all array relevant
        // we don't need code to push the array content onto the stack
        // only the complete array object is required
        // Remove ArraySize * 2 from the code (VALUE Opcode and Index Parameter)
        // By processing it above first the value constants are saved in the program table
        // and can be added to the array object through the addArrayValueCode method
        _targetProgram->removeCode(arrayExpr.getArrayContent().size() * 2);
        _targetProgram->addCode(OpCode::VALUE, arrayExpr.getLine());
        addArrayValueCode(arrayExpr.getArrayContent().size(), arrayExpr.getLine());
    }

    void Compiler::visitIndexExpression(const IndexExpression& indexExpr)
    {
        indexExpr.getLeft().accept(*this);
        indexExpr.getRight().accept(*this);
        _targetProgram->addCode(OpCode::INDEX, indexExpr.getLine());
    }

    void Compiler::visitWhileExpression(const WhileExpression& whileExpr)
    {
        _skipFraming = true;
        _targetProgram->addCode(OpCode::FRAME, whileExpr.getLine());
        float whileStart = _targetProgram->getCode().size();
        whileExpr.getCondition().accept(*this);

        // get code size of loop body to create JUMP code
        auto currentProgram = _targetProgram;
        Program whileProgram;
        _targetProgram = &whileProgram;
        whileExpr.getLoopBody().accept(*this);

        _targetProgram = currentProgram;
        _targetProgram->addCode(OpCode::JMP_NOT, whileExpr.getLine());
        _targetProgram->addCode(OpCode::VALUE, whileExpr.getLine());
        // add JMP, VAL and actual object index to program size (+4.0f)
        addNumberValueCode(_targetProgram->getCode().size() + whileProgram.getCode().size() + 4.0f, whileExpr.getLine());
        whileExpr.getLoopBody().accept(*this);

        _targetProgram->addCode(OpCode::JMP, whileExpr.getLine());
        _targetProgram->addCode(OpCode::VALUE, whileExpr.getLine());
        addNumberValueCode(whileStart, whileExpr.getLine());
        _targetProgram->addCode(OpCode::UNFRAME, whileExpr.getLine());
        _skipFraming = false;
    }

    void Compiler::visitForExpression(const ForExpression& forExpr)
    {
        _skipFraming = true;
        _targetProgram->addCode(OpCode::FRAME, forExpr.getLine());
        forExpr.getAssignStatement().accept(*this);

        float forStart = _targetProgram->getCode().size();
        forExpr.getConditionExpression().accept(*this);

        // get code size of for loop body and incrementstatement to create JUMP code
        auto currentProgram = _targetProgram;
        Program forProgram;
        _targetProgram = &forProgram;
        forExpr.getLoopBody().accept(*this);
        forExpr.getIncrementorStatement().accept(*this);

        _targetProgram = currentProgram;
        _targetProgram->addCode(OpCode::JMP_NOT, forExpr.getLine());
        _targetProgram->addCode(OpCode::VALUE, forExpr.getLine());
        // add JMP, VAL and actual object index to program size (+4.0f)
        addNumberValueCode(_targetProgram->getCode().size() + forProgram.getCode().size() + 4.0f, forExpr.getLine());
        forExpr.getLoopBody().accept(*this);
        forExpr.getIncrementorStatement().accept(*this);

        _targetProgram->addCode(OpCode::JMP, forExpr.getLine());
        _targetProgram->addCode(OpCode::VALUE, forExpr.getLine());
        addNumberValueCode(forStart, forExpr.getLine());
        _targetProgram->addCode(OpCode::UNFRAME, forExpr.getLine());
        _skipFraming = false;
    }

    void Compiler::visitLiteralExpression(const LiteralExpression& literalExpr)
    {
        Token valueToken = literalExpr.getValueToken();
        if (valueToken.getTokenType() == TokenType::IDENTIFIER)
        {
            _targetProgram->addCode(OpCode::LOAD, valueToken.getLine());
            addStrValueCode(valueToken.getLexem(_source), valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::FALSE)
        {
            _targetProgram->addCode(OpCode::VALUE, valueToken.getLine());
            addBoolValueCode(false, valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::TRUE)
        {
            _targetProgram->addCode(OpCode::VALUE, valueToken.getLine());
            addBoolValueCode(true, valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::NUMBER)
        {
            _targetProgram->addCode(OpCode::VALUE, valueToken.getLine());
            addNumberValueCode(valueToken.getNumber(_source), valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::STRING)
        {
            _targetProgram->addCode(OpCode::VALUE, valueToken.getLine());
            addStrValueCode(valueToken.getString(_source), valueToken.getLine());
        }
    }

    void Compiler::addNumberValueCode(float number, uint16_t line)
    {
        _targetProgram->addCode(_targetProgram->addValue(Value(number)), line);
    }

    void Compiler::addBoolValueCode(bool boolean, uint16_t line)
    {
        _targetProgram->addCode(_targetProgram->addValue(Value(boolean)), line);
    }

    void Compiler::addArrayValueCode(uint32_t arraySize, uint16_t line)
    {
        std::vector<Value> arrayContent;
        for (uint32_t i = 1; i <= arraySize; i++)
        {
            // Add the last 'arraySize' count values to the array content
            arrayContent.push_back(_targetProgram->getConstant(_targetProgram->getConstantSize() - i));
        }
        std::reverse(arrayContent.begin(), arrayContent.end());

        auto ptr = std::make_shared<ArrayObj>(arrayContent);
        _targetProgram->addCode(_targetProgram->addValue(Value(ptr.get()), ptr), line);
    }

    void Compiler::addStrValueCode(const std::string& string, uint16_t line)
    {
        auto ptr = std::make_shared<StringObj>(StringObj(string));
        _targetProgram->addCode(_targetProgram->addValue(Value(ptr.get()), ptr), line);
    }

    void Compiler::addFunValueCode(Program&& program, const std::vector<Token>& funParameters, uint16_t line)
    {
        auto ptr = std::make_shared<FunObj>(FunObj(std::move(program)));
        for (const Token& token : funParameters)
        {
            ptr->addParameter(token.getLexem(_source));
        }
        _targetProgram->addCode(_targetProgram->addValue(Value(ptr.get()), ptr), line);
    }

    void Compiler::checkValueCount()
    {
        if (_targetProgram->getConstantSize() > UINT16_MAX)
            _error = std::make_tuple(-1, "Script has to many constants!");
        else if (_targetProgram->getObjectSize() > UINT16_MAX)
            _error = std::make_tuple(-1, "Script has to many objects!");
    }
}