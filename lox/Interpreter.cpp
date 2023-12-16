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
#include "Resolver.h"
#include "LoxClass.h"
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

    Object lookupVariable(Token const& name, Expr const& expr, Environment const& environment, ResolvedLocals const& locals) {
        
        if (auto const it = locals.find(&expr); it != locals.end()) {
            return environment.getAt(it->second, name.lexeme());
        }
        else {
            return Lox::globals.get(name);
        }
    }

    template <class T>
    auto loxCall(Object const& callee, std::vector<Object> const& arguments, Token const& paren) {
        auto const& function = static_cast<T>(callee);

        if (function.arity() != arguments.size()) {
            throw RuntimeError{ paren, "Expected " + std::to_string(function.arity()) + " arguments but got " + std::to_string(arguments.size()) + "." };
        }

        return function(arguments);
    }

    // Forward declaration of generic execute/evaluate:
    Object execute(Stmt const& statement, Environment& environment, ResolvedLocals const& locals);
    Object evaluate(Expr const& expr, Environment& environment, ResolvedLocals const& locals);

    // Evaluate functions of concrete expressions:
    Object evaluateBinaryExpr(BinaryExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const left = evaluate(expr.left(), environment, locals);
        auto const right = evaluate(expr.right(), environment, locals);
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
        default:
            throw RuntimeError{ expr.operatr(), "Sorry I cannot do this!" };
        }
    }
    Object evaluateGroupingExpr(GroupingExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        return evaluate(expr.expression(), environment, locals);
    }
    Object evaluateLiteralExpr(LiteralExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        return expr.value();
    }
    Object evaluateUnaryExpr(UnaryExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const right = evaluate(expr.right(), environment, locals);
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

    Object evaluateVariableExpr(VariableExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        return lookupVariable(expr.name(), expr, environment, locals);
    }

    Object evaluateAssignExpr(AssignExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const value = evaluate(expr.value(), environment, locals);
        
        if (auto const it = locals.find(&expr); it != locals.end()) {
            environment.assignAt(it->second, expr.name(), value);
        }
        else {
            Lox::globals.assign(expr.name(), value);
        }

        return value;
    }
    
    Object evaluateLogicalExpr(LogicalExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const lhs = evaluate(expr.left(), environment, locals);

        if (expr.operatr().tokenType() == TokenType::OR) {
            if (isTruthy(lhs)) return lhs;
        }
        else { // AND
            if (!isTruthy(lhs)) return lhs;
        }

        return evaluate(expr.right(), environment, locals);
    }

    Object evaluateCallExpr(CallExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const callee = evaluate(expr.callee(), environment, locals);
        auto arguments = std::vector<Object>();
        auto const proj = [&](Expr const* expr) { return evaluate(*expr, environment, locals); };
        std::ranges::transform(expr.arguments(), std::back_inserter(arguments), proj);

        if (callee.isLoxCallable()) {
            return loxCall<LoxCallable>(callee, arguments, expr.paren());
        }
        else if(callee.isLoxClass()) {
            return loxCall<LoxClass>(callee, arguments, expr.paren());
        }
        else {
            throw RuntimeError{ expr.paren(), "Can only call functions and classes." };
        }
    }
    Object evaluateGetExpr(GetExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const object = evaluate(expr.object(), environment, locals);
        if (object.isLoxInstance()) {
            return static_cast<LoxInstance>(object).get(expr.name());
        }

        throw RuntimeError{ expr.name(), "Only instances have properties." };
    }
    Object evaluateSetExpr(SetExpr const& expr, Environment& environment, ResolvedLocals const& locals) {
        auto const object = evaluate(expr.object(), environment, locals);
        if (!object.isLoxInstance()) {
            throw RuntimeError{ expr.name(), "Only instances have properties." };
        }

        auto const value = evaluate(expr.value(), environment, locals);
        static_cast<LoxInstance>(object).set(expr.name(), value);
        return value;
    }

    // Execute functions of concrete statements:

    Object executeExpressionStmt(ExpressionStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        return evaluate(stmt.expression(), environment, locals);
    }

    Object executeIfStmt(IfStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto const condition = evaluate(stmt.condition(), environment, locals);
        if (condition)
        {
            execute(stmt.thenBranch(), environment, locals);
        }
        else if (auto const elseBranch = stmt.elseBranch()) {
            execute(*elseBranch, environment, locals);
        }
        return {};
    }

    Object executePrintStmt(PrintStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto const value = evaluate(stmt.expression(), environment, locals);
        std::cout << value.toString() << std::endl;
        return {};
    }
    
    Object executeWhileStmt(WhileStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        while (evaluate(stmt.condition(), environment, locals)) {
            execute(stmt.body(), environment, locals);
        }
        return {};
    }

    Object executeVarStmt(VarStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto const value = stmt.initializer() ? evaluate(*stmt.initializer(), environment, locals) : Object();
        environment.define(stmt.name().lexeme(), value);
        return {};
    }

    Object executeBlockStmt(BlockStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto blockEnvironment = new Environment(&environment);
        auto result = Object{};
        std::ranges::for_each(stmt.statements(), [&](auto const* stmt) {
            assert(stmt && "Statement cannot be null.");
            result = execute(*stmt, *blockEnvironment, locals);
        });
        return result;
    }

    Object executeFunctionStmt(FunctionStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto const function = LoxCallable([&stmt, &environment, &locals](std::vector<Object> const& arguments) {
            auto closure = new Environment(&environment);
            for (auto const& [param, arg] : std::views::zip(stmt.parameters(), arguments)) {
                closure->define(param.lexeme(), arg);
            }
            try {
                executeBlockStmt(stmt.body(), *closure, locals);
            }
            catch (Return const& ret) {
                return ret.object;
            }
            return Object();
            }, static_cast<int>(stmt.parameters().size()), stmt.name().lexeme());
        environment.define(stmt.name().lexeme(), function);
        return {};
    }

    Object executeReturnStmt(ReturnStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        auto const value = stmt.value() ? evaluate(*stmt.value(), environment, locals) : Object{};
        throw Return{ value };
    }

    Object executeClassStmt(ClassStmt const& stmt, Environment& environment, ResolvedLocals const& locals) {
        environment.define(stmt.name().lexeme(), Object());
        auto const klass = LoxClass(stmt.name().lexeme());
        environment.assign(stmt.name(), klass);
        return {};
    }
    

    // Evaluate function of generic expression:

    template <typename T>
    using EvaluateExprFuncT = std::function<Object(T const&, Environment&, ResolvedLocals const&)>;

    Object evaluate(Expr const& expr, Environment& environment, ResolvedLocals const& locals) {
        static auto const evaluateDispatcher = Dispatcher<Object, Expr const&, Environment&, ResolvedLocals const&>(
            EvaluateExprFuncT<BinaryExpr>(evaluateBinaryExpr),
            EvaluateExprFuncT<GroupingExpr>(evaluateGroupingExpr),
            EvaluateExprFuncT<LiteralExpr>(evaluateLiteralExpr),
            EvaluateExprFuncT<UnaryExpr>(evaluateUnaryExpr),
            EvaluateExprFuncT<VariableExpr>(evaluateVariableExpr),
            EvaluateExprFuncT<AssignExpr>(evaluateAssignExpr),
            EvaluateExprFuncT<LogicalExpr>(evaluateLogicalExpr),
            EvaluateExprFuncT<CallExpr>(evaluateCallExpr),
            EvaluateExprFuncT<GetExpr>(evaluateGetExpr),
            EvaluateExprFuncT<SetExpr>(evaluateSetExpr)
        );

        return evaluateDispatcher.dispatch(expr, environment, locals);
    }

    // Execute function of generic statement:

    template <typename T>
    using ExecuteStmtFuncT = std::function<Object(T const&, Environment& environment, ResolvedLocals const&)>;

    Object execute(Stmt const& statement, Environment& environment, ResolvedLocals const& locals) {
        static auto const executeDispatcher = Dispatcher<Object, Stmt const&, Environment&, ResolvedLocals const&>(
            ExecuteStmtFuncT<ExpressionStmt>(executeExpressionStmt),
            ExecuteStmtFuncT<IfStmt>(executeIfStmt),
            ExecuteStmtFuncT<PrintStmt>(executePrintStmt),
            ExecuteStmtFuncT<WhileStmt>(executeWhileStmt),
            ExecuteStmtFuncT<VarStmt>(executeVarStmt),
            ExecuteStmtFuncT<BlockStmt>(executeBlockStmt),
            ExecuteStmtFuncT<FunctionStmt>(executeFunctionStmt),
            ExecuteStmtFuncT<ReturnStmt>(executeReturnStmt),
            ExecuteStmtFuncT<ClassStmt>(executeClassStmt)
        );

        return executeDispatcher.dispatch(statement, environment, locals);
    }
}

Object interpret(std::vector<Stmt const*> const& statements, ResolvedLocals const& locals) {
    try {
        auto result = Object();
        for (auto const* statement : statements) {
            assert(statement && "Statement cannot be nullptr");
            result = execute(*statement, Lox::globals, locals);
        }
        return result;
    }
    catch (RuntimeError const& error) {
        Lox::error(error.token, error.message);
        return {};
    }
}

