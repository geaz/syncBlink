#include "compiler.hpp"

#include "program/model/objects/array_object.hpp"
#include "program/model/objects/function_object.hpp"
#include "program/model/objects/closure_object.hpp"
#include "program/model/objects/string_object.hpp"
#include "program/model/op_codes.hpp"

#include <algorithm>

namespace SyncBlink
{
    Compiler::Compiler(std::shared_ptr<ScriptSource> source, const ProgramAst& programAst) : _source(source), _programAst(programAst)
    {
    }

    Program Compiler::compile()
    {
        _error = std::tuple<LINETYPE, std::string>(0, "");
        for (auto& node : _programAst.getNodes())
        {
            node->accept(*this);
            checkProgramSizes();

            if (hasError()) break;
        } 
        checkReturnValue();
        return std::move(_program);
    }

    bool Compiler::hasError() const
    {
        return std::get<1>(_error) != "";
    }

    std::tuple<LINETYPE, std::string> Compiler::getError() const
    {
        return _error;
    }

    void Compiler::visitExpressionStatement(const ExpressionStatement& expressionStatement)
    {
        expressionStatement.getExpression().accept(*this);
        // Statements don't leave values on stack
        addCode(OpCode::POP, 0);
    }

    void Compiler::visitLetStatement(const LetStatement& letStatement)
    {
        letStatement.getExpression().accept(*this);
        std::string identifier = letStatement.getIdentifier().getLexem(_source);
        if(_localScope->isGlobalScope())
        {
            if(std::find(_globalIdentifiers.begin(), _globalIdentifiers.end(), identifier) == _globalIdentifiers.end())
            {
                _globalIdentifiers.push_back(identifier);
                addCode(OpCode::DEFINE_GLOBAL, letStatement.getIdentifier().getLine());
                addStrValueCode(identifier, letStatement.getIdentifier().getLine());  
            }
            else
                _error = std::make_tuple(letStatement.getIdentifier().getLine(), "Global variable already defined!");            
        }
        else if(!_localScope->isLocalDefined(identifier))
        {
            _localScope->addIdentifier(identifier);
            // Nothing to add to the stack! The previously pushed Value IS the local variable
            // Future loads of the local variable will reference the position of the previously pushed value
            // All local variables living on the stack! And the stack position of the value is equal to the
            // localValue vector size! (all because of how the stack gets manipulated by the VM)
        }
        else
            _error = std::make_tuple(letStatement.getIdentifier().getLine(), "Local variable already defined!");         
    }

    void Compiler::visitGroupExpression(const GroupExpression& groupExpr)
    {
        groupExpr.getExpression().accept(*this);
    }

    void Compiler::visitBlockExpression(const BlockExpression& blockExpression)
    {
        // if block expression is empty, just return a nil value
        if (blockExpression.getStatements().size() == 0)
        {
            addCode(OpCode::VALUE, 0);
            addCode(_targetProgram->addConstant(Value()), 0);
            return;
        }

        _localScope = std::make_shared<LocalScope>(_localScope);
        
        for (auto& node : blockExpression.getStatements())
        {
            node->accept(*this);
        }

        // blocks are expression and leave a value on the stack (implicit return value)
        checkReturnValue();
        removeFrame();
    }

    void Compiler::visitAssignExpression(const AssignExpression& assignExpression)
    {
        assignExpression.getExpression().accept(*this);

        std::string identifier = assignExpression.getIdentifier().getLexem(_source);

        LocalValue localValue;
        if (!_localScope->isGlobalScope() && _localScope->getLocalValue(identifier, localValue))
        {
            addCode(OpCode::SET_LOCAL, assignExpression.getIdentifier().getLine());
            addNumberValueCode(localValue.index, assignExpression.getIdentifier().getLine());
        }
        else // Always set a global, if a local variable was not found - it could be a variable which
            // is only known during runtime (added through native code)
        {
            addCode(OpCode::SET_GLOBAL, assignExpression.getIdentifier().getLine());
            addStrValueCode(identifier, assignExpression.getIdentifier().getLine());  
        }
    }

    void Compiler::visitArrayAssignExpression(const ArrayAssignExpression& arrayAssignExpression)
    {
        arrayAssignExpression.getExpression().accept(*this);
        auto indexExpr = static_cast<const IndexExpression*>(arrayAssignExpression.getIndexPtr());
        indexExpr->getLeft().accept(*this);
        indexExpr->getRight().accept(*this);
        addCode(OpCode::SET_INDEX, arrayAssignExpression.getLine());
    }

    void Compiler::visitPrefixExpression(const PrefixExpression& prefixExpr)
    {
        prefixExpr.getExpression().accept(*this);
        switch (prefixExpr.getOperatorToken().getTokenType())
        {
        case TokenType::BANG:
            addCode(OpCode::BANG_NEGATE, prefixExpr.getOperatorToken().getLine());
            break;
        case TokenType::MINUS:
            addCode(OpCode::NEGATE, prefixExpr.getOperatorToken().getLine());
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
            addCode(OpCode::ADD, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::MINUS:
            addCode(OpCode::SUB, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::SLASH:
            addCode(OpCode::DIV, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::STAR:
            addCode(OpCode::MUL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::MODULO:
            addCode(OpCode::MODULO, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::AND:
            addCode(OpCode::AND, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::OR:
            addCode(OpCode::OR, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::BANG_EQUAL:
            addCode(OpCode::BANG_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::LESS:
            addCode(OpCode::LESS, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::LESS_EQUAL:
            addCode(OpCode::LESS_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::GREATER:
            addCode(OpCode::GREATER, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::GREATER_EQUAL:
            addCode(OpCode::GREATER_EQUAL, infixExpr.getOperatorToken().getLine());
            break;
        case TokenType::EQUAL_EQUAL:
            addCode(OpCode::EQUAL_EQUAL, infixExpr.getOperatorToken().getLine());
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

        Program ifProgram;
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
        // Add Jmp over if body, if condition is false
        addCode(OpCode::JMP_NOT, conditionExpr.getLine());
        addCode(OpCode::VALUE, conditionExpr.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + ifProgram.getCode().size() + 4.0f /* jump over jump code of else branch */, conditionExpr.getLine());

        // add if branch code
        conditionExpr.getIfBranch().accept(*this);

        if (elsePresent)
        {
            // Add JMP beyond else branch, if the if-branch was executed
            addCode(OpCode::JMP, conditionExpr.getLine());
            addCode(OpCode::VALUE, conditionExpr.getLine());
            addNumberValueCode(_targetProgram->getCode().size() + elseProgram.getCode().size() + 1.0f, conditionExpr.getLine());

            conditionExpr.getElseBranch()->accept(*this);
        }
        else
        {
            // Add JMP beyond nil branch, if the if-branch was executed
            addCode(OpCode::JMP, conditionExpr.getLine());
            addCode(OpCode::VALUE, conditionExpr.getLine());
            addNumberValueCode(_targetProgram->getCode().size() + 3.0f, conditionExpr.getLine());

            // add nil return value if no else branch is present
            addCode(OpCode::VALUE, conditionExpr.getLine());
            addCode(_targetProgram->addConstant(Value()), conditionExpr.getLine());
        }
    }

    void Compiler::visitWhileExpression(const WhileExpression& whileExpression)
    {
        // get code size of loop body and condition to create jumps
        auto currentProgram = _targetProgram;
        
        Program whileProgram;
        _targetProgram = &whileProgram;
        whileExpression.getLoopBody().accept(*this);

        Program conditionProgam;
        _targetProgram = &conditionProgam;
        whileExpression.getCondition().accept(*this);

        _targetProgram = currentProgram;

        // compile condition statement just for a single execution
        // to check, if the condition is already unmet
        whileExpression.getCondition().accept(*this);

        // if the condition is already unmet, the while loop will not execute
        // and we jump to the position to add a NIL value to the stack as the return value
        addCode(OpCode::JMP_NOT, whileExpression.getLine());
        addCode(OpCode::VALUE, whileExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + whileProgram.getCode().size() + conditionProgam.getCode().size() + 11.0f,
                           whileExpression.getLine());

        // Add POP which gets executed after each loop execution but the last and the first
        // to clear the block return value from the stack. Jump over it the first  time.
        addCode(OpCode::JMP, whileExpression.getLine());
        addCode(OpCode::VALUE, whileExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + 2.0f, whileExpression.getLine());
        addCode(OpCode::POP, whileExpression.getLine());
        uint16_t popLocation = (uint16_t)_targetProgram->getCode().size() - 1;

        // compile loop body
        whileExpression.getLoopBody().accept(*this);

        // compile condition statement to check the condition after the loop execution
        whileExpression.getCondition().accept(*this);

        // if false, jump over to the end of the whole loop
        addCode(OpCode::JMP_NOT, whileExpression.getLine());
        addCode(OpCode::VALUE, whileExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + 6.0f, whileExpression.getLine());

        // otherwise jump into the pop value for the next loop execution
        addCode(OpCode::JMP, whileExpression.getLine());
        addCode(OpCode::VALUE, whileExpression.getLine());
        addNumberValueCode(popLocation, whileExpression.getLine());

        // this nil value is just in case the whole loop does not execute a single time
        addCode(OpCode::VALUE, whileExpression.getLine());
        addCode(_targetProgram->addConstant(Value()), whileExpression.getLine());
    }

    void Compiler::visitForExpression(const ForExpression& forExpression)
    {
        // Frame the for loop to get rid of the
        // loop condition identifiers after execution
        // the loop body gets framed automatically too (block statement) to get rid of the
        // locals after each loop call
        _localScope = std::make_shared<LocalScope>(_localScope);

        // get code size of for loop body and incrementstatement to create JUMP code,
        // if the condition evaluates to false
        auto currentProgram = _targetProgram;

        Program forProgram;
        _targetProgram = &forProgram;
        forExpression.getLoopBody().accept(*this);

        Program conditionProgam;
        _targetProgram = &conditionProgam;
        forExpression.getConditionExpression().accept(*this);

        Program getIncrementorProgram;
        _targetProgram = &getIncrementorProgram;
        forExpression.getIncrementorStatement().accept(*this);

        _targetProgram = currentProgram;

        // compile assignment
        forExpression.getAssignStatement().accept(*this);

        // compile condition statement just for a single execution
        // to check, if the condition is already unmet
        forExpression.getConditionExpression().accept(*this);

        // if the condition is already unmet, the loop will not execute
        // and we jump to the position to add a NIL value to the stack as the return value
        addCode(OpCode::JMP_NOT, forExpression.getLine());
        addCode(OpCode::VALUE, forExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + forProgram.getCode().size() + 
            getIncrementorProgram.getCode().size() + conditionProgam.getCode().size() + 11.0f, forExpression.getLine());

        // Add POP which gets executed after each loop execution but the last and the first
        // to clear the block return value from the stack. Jump over it the first  time.
        addCode(OpCode::JMP, forExpression.getLine());
        addCode(OpCode::VALUE, forExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + 2.0f, forExpression.getLine());
        addCode(OpCode::POP, forExpression.getLine());
        uint16_t popLocation = (uint16_t)_targetProgram->getCode().size() - 1;

        // compile loop body
        forExpression.getLoopBody().accept(*this);

        // compile incrementation and condition to check the condition after the loop execution
        forExpression.getIncrementorStatement().accept(*this);
        forExpression.getConditionExpression().accept(*this);

        // if false, jump over to the end of the whole loop
        addCode(OpCode::JMP_NOT, forExpression.getLine());
        addCode(OpCode::VALUE, forExpression.getLine());
        addNumberValueCode(_targetProgram->getCode().size() + 6.0f, forExpression.getLine());

        // otherwise jump into the pop value for the next loop execution
        addCode(OpCode::JMP, forExpression.getLine());
        addCode(OpCode::VALUE, forExpression.getLine());
        addNumberValueCode(popLocation, forExpression.getLine());

        // this nil value is just in case the whole loop does not execute a single time
        addCode(OpCode::VALUE, forExpression.getLine());
        addCode(_targetProgram->addConstant(Value()), forExpression.getLine());

        removeFrame();
    }

    void Compiler::visitFunctionExpression(const FunctionExpression& functionExpr)
    {
        auto currentProgram = _targetProgram;
        Program funProgram;

        _targetProgram = &funProgram;

        // We are going to parse the block statement
        // "manually", because we have to
        // scope the function parameters too
        // and do some work after the block scope has ended
        _localScope = std::make_shared<LocalScope>(_localScope, true);

        // Add the parameters as locals on the function scope
        for(Token parameter : functionExpr.getParameters())
        {
            std::string identifier = parameter.getLexem(_source);
            _localScope->addIdentifier(identifier);
        }

        // compile function body "manually"
        auto bodyAst = static_cast<const BlockExpression*>(functionExpr.getFunctionBodyPtr());
        for (auto& node : bodyAst->getStatements())
        {
            node->accept(*this);
        }

        checkReturnValue();
        auto removedScope = removeFrame();

        // Create the function object and add the parameters
        auto funPtr = std::make_shared<FunObj>(FunObj(std::move(funProgram)));
        for (const Token& token : functionExpr.getParameters())
        {
            funPtr->addParameter(token.getLexem(_source));
        }

        // Create a array of all foreign indices
        // These are the positions of the enclosed local values on the stack
        // The moment a function object gets loaded, the VM creates a Closure which
        // includes the values on those positions at the time the closure got created.
        // On a CALL operation the VM will push them back on the stack as locals for the function to use
        // These foreign locals will then get used like all other local values.
        std::vector<size_t> foreignLocalIndices;
        for (LocalValue localValue : removedScope->getLocalValues())
        {
            if (localValue.isForeignLocal) foreignLocalIndices.push_back(localValue.foreignIndex);
        }
        funPtr->setForeignLocalIndices(foreignLocalIndices);
        
        _targetProgram = currentProgram;
        addCode(OpCode::CLOSURE, functionExpr.getLine());
        addCode(_targetProgram->addConstant(Value(funPtr.get()), funPtr), functionExpr.getLine());
    }

    void Compiler::visitCallExpression(const CallExpression& callExpr)
    {
        for (size_t i = 0; i < callExpr.getParameters().size(); i++)
        {
            callExpr.getParameters()[i]->accept(*this);
        }

        addLoadVariableCode(callExpr.getIdentifier().getLexem(_source), callExpr.getIdentifier().getLine());
        addCode(OpCode::CALL, callExpr.getIdentifier().getLine());
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
        _targetProgram->removeCode((MAXCODE)arrayExpr.getArrayContent().size() * 2);
        addCode(OpCode::VALUE, arrayExpr.getLine());
        addArrayValueCode((MAXITEM)arrayExpr.getArrayContent().size(), arrayExpr.getLine());
    }

    void Compiler::visitIndexExpression(const IndexExpression& indexExpr)
    {
        indexExpr.getLeft().accept(*this);
        indexExpr.getRight().accept(*this);
        addCode(OpCode::INDEX, indexExpr.getLine());
    }

    void Compiler::visitLiteralExpression(const LiteralExpression& literalExpr)
    {
        Token valueToken = literalExpr.getValueToken();
        if (valueToken.getTokenType() == TokenType::IDENTIFIER)
            addLoadVariableCode(valueToken.getLexem(_source), valueToken.getLine());
        else if (valueToken.getTokenType() == TokenType::FALSE)
        {
            addCode(OpCode::VALUE, valueToken.getLine());
            addBoolValueCode(false, valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::TRUE)
        {
            addCode(OpCode::VALUE, valueToken.getLine());
            addBoolValueCode(true, valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::NUMBER)
        {
            addCode(OpCode::VALUE, valueToken.getLine());
            addNumberValueCode(valueToken.getNumber(_source), valueToken.getLine());
        }
        else if (valueToken.getTokenType() == TokenType::STRING)
        {
            addCode(OpCode::VALUE, valueToken.getLine());
            addStrValueCode(valueToken.getString(_source), valueToken.getLine());
        }
    }

    void Compiler::addCode(CODETYPE code, size_t line)
    {
        if(!checkLineSize(line)) return;
        _targetProgram->addCode(code, (LINETYPE)line);
    }

    void Compiler::addNumberValueCode(float number, size_t line)
    {
        if(!checkLineSize(line)) return;
        _targetProgram->addCode(_targetProgram->addConstant(Value(number)), (LINETYPE)line);
    }

    void Compiler::addBoolValueCode(bool boolean, size_t line)
    {
        if(!checkLineSize(line)) return;
        _targetProgram->addCode(_targetProgram->addConstant(Value(boolean)), (LINETYPE)line);
    }

    void Compiler::addArrayValueCode(MAXITEM arraySize, size_t line)
    {
        if(!checkLineSize(line)) return;

        std::vector<Value> arrayContent;
        for (MAXITEM i = 1; i <= arraySize; i++)
        {
            // Add the last 'arraySize' count values to the array content
            arrayContent.push_back(_targetProgram->getConstants()[_targetProgram->getConstants().size() - i]);
        }
        std::reverse(arrayContent.begin(), arrayContent.end());

        auto ptr = std::make_shared<ArrayObj>(arrayContent);
        _targetProgram->addCode(_targetProgram->addConstant(Value(ptr.get()), ptr), (LINETYPE)line);
    }

    void Compiler::addStrValueCode(const std::string& string, size_t line)
    {
        if(!checkLineSize(line)) return;
        auto ptr = std::make_shared<StringObj>(StringObj(string));
        _targetProgram->addCode(_targetProgram->addConstant(Value(ptr.get()), ptr), (LINETYPE)line);
    }

    void Compiler::addLoadVariableCode(const std::string& identifier, size_t line)
    {        
        if(!checkLineSize(line)) return;

        LocalValue localValue;
        if (!_localScope->isGlobalScope() && _localScope->getLocalValue(identifier, localValue))
        {
            _targetProgram->addCode(OpCode::LOAD_LOCAL, (LINETYPE)line);
            addNumberValueCode(localValue.index, line);
        }
        // The identifier could be unknown now, because it will referencean external function/variable during runtime
        else
        {
            _targetProgram->addCode(OpCode::LOAD_GLOBAL, (LINETYPE)line);
            addStrValueCode(identifier, line);  
        }
    }

    std::shared_ptr<LocalScope> Compiler::removeFrame()
    {
        uint16_t addedLocals = (uint16_t)_localScope->getLocalValues().size() - (uint16_t)_localScope->parentScope->getLocalValues().size();
        if (_localScope->isForeignScope()) addedLocals = (uint16_t)_localScope->getLocalValues().size();

        _targetProgram->addCode(OpCode::CLEAR_SCOPE, 0);
        _targetProgram->addCode(_targetProgram->addConstant(Value((float)addedLocals)), 0);

        auto removedScope = _localScope;
        _localScope = _localScope->parentScope;

        return removedScope;
    }
    
    void Compiler::checkReturnValue()
    {
        // Keep the last expression statement on stack as a return value
        if(_targetProgram->getCode().size() > 0 && _targetProgram->getCode().back() == OpCode::POP)
            _targetProgram->removeCode(1);
        else // If no POP was added, the Script is not returning any values (last line was an expression) Add a nil value as return
        {
            _targetProgram->addCode(OpCode::VALUE, 0);
            _targetProgram->addCode(_targetProgram->addConstant(Value()), 0);
        }
    }

    void Compiler::checkProgramSizes()
    {
        if (_targetProgram->getConstants().size() >= UINT8_MAX + 1) _error = std::make_tuple(0, "Script has to many constants!");
        else if (_targetProgram->getObjects().size() >= UINT8_MAX + 1)  _error = std::make_tuple(0, "Script has to many objects!");
        else if (_targetProgram->getCode().size() >= UINT16_MAX + 1)  _error = std::make_tuple(0, "Script is to large!");
    }

    bool Compiler::checkLineSize(size_t line)
    {
        bool lineOk = true;
        if (line > LINETYPEMAX)
        {
            _error = std::make_tuple(LINETYPEMAX, "Maximum of lines reached!");
            lineOk = false;
        }
        return lineOk;
    }
}