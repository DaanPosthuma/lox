#include "Interpreter.h"
#include "Object.h"
#include "Expr.h"
#include "Dispatcher.h"
#include "TokenType.h"
#include "Lox.h"
#include "Stmt.h"
#include "RuntimeError.h"
#include "Environment.h"
#include <stdexcept>
#include <cassert>
#include <iostream>

namespace {

    bool isTruthy(Object const& object) {
        if (object.isNil()) return false;
        if (object.isBoolean()) return (bool)object;
        return true;
    }

    void checkNumberOperand(Token const& token, Object const& object) {
        if (!object.isDouble()) throw RuntimeError(token, "Operand must be a number.");
    }

    void checkNumberOperands(Token const& token, Object const& left, Object const& right) {
        if (!left.isDouble() || !right.isDouble()) throw RuntimeError(token, "Operands must be numbers.");
    }

    // forward declaration of generic execute/evaluate:
    void execute(Stmt const& statement, Environment& environment);
    Object evaluate(Expr const& expr, Environment& environment);

    // Evaluate functions of concrete expressions:
    Object evaluateBinaryExpr(BinaryExpr const& expr, Environment& environment) {
        auto const left = evaluate(expr.getLeft(), environment);
        auto const right = evaluate(expr.getRight(), environment);
        auto const& operatr = expr.getOperator();

        switch (operatr.getTokenType()) {
        case TokenType::MINUS:
            checkNumberOperands(operatr, left, right);
            return (double)left - (double)right;
        case TokenType::PLUS:
            
            if (left.isString() || right.isString())
                return left.toString() + right.toString();

            if (left.isDouble() && right.isDouble())
                return (double)left + (double)right;

            throw RuntimeError(operatr, "Cannot concatenate " + left.toString() + " and " + right.toString() + ".");
        case TokenType::SLASH:
            checkNumberOperands(operatr, left, right);
            return (double)left / (double)right;
        case TokenType::STAR:
            checkNumberOperands(operatr, left, right);
            return (double)left * (double)right;
        case TokenType::BANG_EQUAL:
            return left != right;
        case TokenType::EQUAL_EQUAL:
            return left == right;
        case TokenType::GREATER:
            checkNumberOperands(operatr, left, right);
            return (double)left > (double)right;
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(operatr, left, right);
            return (double)left >= (double)right;
        case TokenType::LESS:
            checkNumberOperands(operatr, left, right);
            return (double)left < (double)right;
        case TokenType::LESS_EQUAL:
            checkNumberOperands(operatr, left, right);
            return (double)left <= (double)right;
        }

        throw RuntimeError(expr.getOperator(), "Sorry I cannot do this!");
    }
    Object evaluateGroupingExpr(GroupingExpr const& expr, Environment& environment) {
        return evaluate(expr.getExpression(), environment);
    }
    Object evaluateLiteralExpr(LiteralExpr const& expr, Environment& environment) {
        return expr.getValue();
    }
    Object evaluateUnaryExpr(UnaryExpr const& expr, Environment& environment) {
        auto const right = evaluate(expr.getRight(), environment);
        auto const& operatr = expr.getOperator();

        switch (operatr.getTokenType()) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            checkNumberOperand(operatr, right);
            return -(double)right;
            break;
        default:
            throw std::logic_error("what happen?");
        }
    }

    Object evaluateVariableExpr(VariableExpr const& expr, Environment& environment) {
        return environment.get(expr.getName());
    }

    Object evaluateAssignExpr(AssignExpr const& expr, Environment& environment) {
        auto const value = evaluate(expr.getValue(), environment);
        environment.assign(expr.getName(), value);
        return value;
    }

    // Execute functions of concrete statements:
    void executeExpressionStmt(ExpressionStmt const& stmt, Environment& environment) {
        evaluate(stmt.getExpression(), environment);
    }

    void executePrintStmt(PrintStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.getExpression(), environment);
        std::cout << value.toString() << std::endl;
    }

    void executeVarStmt(VarStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.getInitializer(), environment);
        environment.define(stmt.getToken().getLexeme(), value);
    }

    // Evaluate function of generic expression:

    template <typename T>
    using EvaluateExprFuncT = std::function<Object(T const&, Environment&)>;

    Object evaluate(Expr const& expr, Environment& environment) {
        static auto const evaluateDispatcher = Dispatcher<Object, Expr const&, Environment&>(
            EvaluateExprFuncT<BinaryExpr>(evaluateBinaryExpr),
            EvaluateExprFuncT<GroupingExpr>(evaluateGroupingExpr),
            EvaluateExprFuncT<LiteralExpr>(evaluateLiteralExpr),
            EvaluateExprFuncT<UnaryExpr>(evaluateUnaryExpr),
            EvaluateExprFuncT<VariableExpr>(evaluateVariableExpr),
            EvaluateExprFuncT<AssignExpr>(evaluateAssignExpr)
        );

        return evaluateDispatcher.dispatch(expr, environment);
    }

    // Execute function of generic statement:

    template <typename T>
    using ExecuteStmtFuncT = std::function<void(T const&, Environment& environment)>;

    void execute(Stmt const& statement, Environment& environment) {
        static auto const executeDispatcher = Dispatcher<void, Stmt const&, Environment&>(
            ExecuteStmtFuncT<ExpressionStmt>(executeExpressionStmt),
            ExecuteStmtFuncT<PrintStmt>(executePrintStmt),
            ExecuteStmtFuncT<VarStmt>(executeVarStmt)
        );

        return executeDispatcher.dispatch(statement, environment);
    }
}

void interpret(std::vector<Stmt*> const& statements, Environment& environment) {
    try {
        for (auto const* statement : statements) {
            assert(statement && "Statement cannot be nullptr");
            execute(*statement, environment);
        }
    }
    catch (RuntimeError const& error) {
        Lox::error(error.token, error.message);
    }
}

