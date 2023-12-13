#include "Interpreter.h"
#include "Object.h"
#include "Expr.h"
#include "Dispatcher.h"
#include "TokenType.h"
#include "Lox.h"
#include "Stmt.h"
#include "RuntimeError.h"
#include "Environment.h"
#include "LoxCallable.h"
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <ranges>

namespace {

    struct Return {
        Object object;
    };

    // Utility functions used in the concrete execute/evaluate functions

    bool isTruthy(Object const& object) {
        if (object.isNil()) return false;
        if (object.isBoolean()) return (bool)object;
        return true;
    }

    void checkNumberOperand(Token const& token, Object const& object) {
        if (!object.isDouble()) throw RuntimeError{token, "Operand must be a number."};
    }

    void checkNumberOperands(Token const& token, Object const& left, Object const& right) {
        if (!left.isDouble() || !right.isDouble()) throw RuntimeError{token, "Operands must be numbers."};
    }

    // Forward declaration of generic execute/evaluate:
    Object execute(Stmt const& statement, Environment& environment);
    Object evaluate(Expr const& expr, Environment& environment);

    // Evaluate functions of concrete expressions:
    Object evaluateBinaryExpr(BinaryExpr const& expr, Environment& environment) {
        auto const left = evaluate(expr.left(), environment);
        auto const right = evaluate(expr.right(), environment);
        auto const& operatr = expr.operatr();

        switch (operatr.tokenType()) {
        case TokenType::MINUS:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) - static_cast<double>(right);
        case TokenType::PLUS:
            
            if (left.isString() || right.isString())
                return left.toString() + right.toString();

            if (left.isDouble() && right.isDouble())
                return static_cast<double>(left) + static_cast<double>(right);

            throw RuntimeError{operatr, "Cannot concatenate " + left.toString() + " and " + right.toString() + "."};
        case TokenType::SLASH:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) / static_cast<double>(right);
        case TokenType::STAR:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) * static_cast<double>(right);
        case TokenType::BANG_EQUAL:
            return left != right;
        case TokenType::EQUAL_EQUAL:
            return left == right;
        case TokenType::GREATER:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) > static_cast<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) >= static_cast<double>(right);
        case TokenType::LESS:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) < static_cast<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(operatr, left, right);
            return static_cast<double>(left) <= static_cast<double>(right);
        }

        throw RuntimeError{expr.operatr(), "Sorry I cannot do this!"};
    }
    Object evaluateGroupingExpr(GroupingExpr const& expr, Environment& environment) {
        return evaluate(expr.expression(), environment);
    }
    Object evaluateLiteralExpr(LiteralExpr const& expr, Environment& environment) {
        return expr.value();
    }
    Object evaluateUnaryExpr(UnaryExpr const& expr, Environment& environment) {
        auto const right = evaluate(expr.right(), environment);
        auto const& operatr = expr.operatr();

        switch (operatr.tokenType()) {
        case TokenType::BANG:
            return !isTruthy(right);
        case TokenType::MINUS:
            checkNumberOperand(operatr, right);
            return -static_cast<double>(right);
            break;
        default:
            throw std::logic_error("what happen?");
        }
    }

    Object evaluateVariableExpr(VariableExpr const& expr, Environment& environment) {
        return environment.get(expr.name());
    }

    Object evaluateAssignExpr(AssignExpr const& expr, Environment& environment) {
        auto const value = evaluate(expr.value(), environment);
        environment.assign(expr.name(), value);
        return value;
    }
    
    Object evaluateLogicalExpr(LogicalExpr const& expr, Environment& environment) {
        auto const lhs = evaluate(expr.left(), environment);

        if (expr.operatr().tokenType() == TokenType::OR) {
            if (isTruthy(lhs)) return lhs;
        }
        else { // AND
            if (!isTruthy(lhs)) return lhs;
        }

        return evaluate(expr.right(), environment);
    }

    Object evaluateCallExpr(CallExpr const& expr, Environment& environment) {
        auto const callee = evaluate(expr.callee(), environment);
        auto arguments = std::vector<Object>();
        auto const proj = [&](Expr const* expr) { return evaluate(*expr, environment); };
        std::ranges::transform(expr.arguments(), std::back_inserter(arguments), proj);

        if (!callee.isLoxCallable()) {
            throw RuntimeError{expr.paren(), "Operand must be a number."};
        }
        auto const& function = static_cast<LoxCallable>(callee);

        if (function.arity() != arguments.size()) {
            throw RuntimeError{expr.paren(), "Expected " + std::to_string(function.arity()) + " arguments but got " + std::to_string(arguments.size()) + "."};
        }

        return function(arguments);
    }

    // Execute functions of concrete statements:

    Object executeExpressionStmt(ExpressionStmt const& stmt, Environment& environment) {
        return evaluate(stmt.expression(), environment);
    }

    Object executeIfStmt(IfStmt const& stmt, Environment& environment) {
        auto const condition = evaluate(stmt.condition(), environment);
        if (condition)
        {
            execute(stmt.thenBranch(), environment);
        }
        else if (auto const elseBranch = stmt.elseBranch()) {
            execute(*elseBranch, environment);
        }
        return {};
    }

    Object executePrintStmt(PrintStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.expression(), environment);
        std::cout << value.toString() << std::endl;
        return {};
    }
    
    Object executeWhileStmt(WhileStmt const& stmt, Environment& environment) {
        while (evaluate(stmt.condition(), environment)) {
            execute(stmt.body(), environment);
        }
        return {};
    }

    Object executeVarStmt(VarStmt const& stmt, Environment& environment) {
        auto const value = evaluate(stmt.initializer(), environment);
        environment.define(stmt.token().lexeme(), value);
        return {};
    }

    Object executeBlockStmt(BlockStmt const& stmt, Environment& environment) {
        auto blockEnvironment = new Environment(&environment);
        auto result = Object{};
        std::ranges::for_each(stmt.statements(), [&](auto const* stmt) {
            assert(stmt && "Statement cannot be null.");
            result = execute(*stmt, *blockEnvironment);
        });
        return result;
    }

    Object executeFunctionStmt(FunctionStmt const& stmt, Environment& environment) {
        auto const function = LoxCallable([&stmt, &environment](std::vector<Object> const& arguments) {
            auto closure = new Environment(&environment);
            for (auto const& [param, arg] : std::views::zip(stmt.parameters(), arguments)) {
                closure->define(param.lexeme(), arg);
            }
            try {
                executeBlockStmt(stmt.body(), *closure);
            }
            catch (Return const& ret) {
                return ret.object;
            }
            return Object();
            }, static_cast<int>(stmt.parameters().size()), stmt.name().lexeme());
        environment.define(stmt.name().lexeme(), function);
        return {};
    }

    Object executeReturnStmt(ReturnStmt const& stmt, Environment& environment) {
        auto const value = stmt.value() ? evaluate(*stmt.value(), environment) : Object{};
        throw Return{value};
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
            EvaluateExprFuncT<LogicalExpr>(evaluateLogicalExpr),
            EvaluateExprFuncT<CallExpr>(evaluateCallExpr)
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
            ExecuteStmtFuncT<BlockStmt>(executeBlockStmt),
            ExecuteStmtFuncT<FunctionStmt>(executeFunctionStmt),
            ExecuteStmtFuncT<ReturnStmt>(executeReturnStmt)
        );

        return executeDispatcher.dispatch(statement, environment);
    }
}

Object interpret(std::vector<Stmt const*> const& statements) {
    try {
        auto result = Object();
        for (auto const* statement : statements) {
            assert(statement && "Statement cannot be nullptr");
            result = execute(*statement, Lox::globals);
        }
        return result;
    }
    catch (RuntimeError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}

