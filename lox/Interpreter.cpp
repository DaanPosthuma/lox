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
#include <algorithm>

namespace {

    // Utility functions used in the concrete execute/evaluate functions

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

    // Forward declaration of generic execute/evaluate:
    Object execute(Stmt const& statement, Environment& environment);
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
    
    Object evaluateLogicalExpr(LogicalExpr const& expr, Environment& environment) {
        auto const lhs = evaluate(expr.getLeft(), environment);

        if (expr.getOperator().getTokenType() == TokenType::OR) {
            if (isTruthy(lhs)) return lhs;
        }
        else { // AND
            if (!isTruthy(lhs)) return lhs;
        }

        return evaluate(expr.getRight(), environment);
    }

    // Execute functions of concrete statements:

    Object executeExpressionStmt(ExpressionStmt const& stmt, Environment& environment) {
        return evaluate(stmt.getExpression(), environment);
    }

    Object executeIfStmt(IfStmt const& stmt, Environment& environment) {
        auto const condition = evaluate(stmt.getCondition(), environment);
        if (condition)
        {
            execute(stmt.getThenBranch(), environment);
        }
        else if (auto const elseBranch = stmt.getElseBranch()) {
            execute(*elseBranch, environment);
        }
        return {};
    }

    Object executePrintStmt(PrintStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.getExpression(), environment);
        std::cout << value.toString() << std::endl;
        return {};
    }
    
    Object executeWhileStmt(WhileStmt const& stmt, Environment& environment) {
        while (evaluate(stmt.getCondition(), environment)) {
            execute(stmt.getBody(), environment);
        }
        return {};
    }

    Object executeVarStmt(VarStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.getInitializer(), environment);
        environment.define(stmt.getToken().getLexeme(), value);
        return {};
    }

    Object executeBlockStmt(BlockStmt const& stmt, Environment& environment) {
        auto blockEnvironment = Environment(environment);
        auto result = Object{};
        std::ranges::for_each(stmt.getStatements(), [&](auto const* stmt) {
            assert(stmt && "Statement cannot be null.");
            result = execute(*stmt, blockEnvironment);
            });
        return result;
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
            EvaluateExprFuncT<AssignExpr>(evaluateAssignExpr),
            EvaluateExprFuncT<LogicalExpr>(evaluateLogicalExpr)
        );

        return evaluateDispatcher.dispatch(expr, environment);
    }

    // Execute function of generic statement:

    template <typename T>
    using ExecuteStmtFuncT = std::function<Object(T const&, Environment& environment)>;

    Object execute(Stmt const& statement, Environment& environment) {
        static auto const executeDispatcher = Dispatcher<Object, Stmt const&, Environment&>(
            ExecuteStmtFuncT<ExpressionStmt>(executeExpressionStmt),
            ExecuteStmtFuncT<IfStmt>(executeIfStmt),
            ExecuteStmtFuncT<PrintStmt>(executePrintStmt),
            ExecuteStmtFuncT<WhileStmt>(executeWhileStmt),
            ExecuteStmtFuncT<VarStmt>(executeVarStmt),
            ExecuteStmtFuncT<BlockStmt>(executeBlockStmt)
        );

        return executeDispatcher.dispatch(statement, environment);
    }
}

Object interpret(std::vector<Stmt*> const& statements, Environment& environment) {
    try {
        auto result = Object();
        for (auto const* statement : statements) {
            assert(statement && "Statement cannot be nullptr");
            result = execute(*statement, environment);
        }
        return result;
    }
    catch (RuntimeError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}

