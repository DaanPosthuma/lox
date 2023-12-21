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

    Object lookupVariable(Token const& name, Expr const& expr, Environment const& environment) {
        
        if (auto const it = Lox::locals.find(&expr); it != Lox::locals.end()) {
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

    Object executeBlockStmt(BlockStmt const& stmt, Environment& environment);

    auto loxCallableFromFunctionStmt(FunctionStmt const& stmt, Environment& environment, std::string const& className = "") {
        auto const isInitializer = !className.empty() && stmt.name().lexeme() == "init";
        auto executeFun = [&stmt,isInitializer](Environment* closure, std::vector<Object> const& arguments) {
            auto environment = new Environment(closure);
            for (auto const& [param, arg] : std::views::zip(stmt.parameters(), arguments)) {
                environment->define(param.lexeme(), arg);
            }
            try {
                executeBlockStmt(stmt.body(), *environment);
            }
            catch (Return const& ret) {
                return isInitializer ? closure->getAt(0, "this") : ret.object;
            }
            return isInitializer ? closure->getAt(0, "this") : Object();
        };

        auto const functionName = className.empty() ? stmt.name().lexeme() : className + "::" + stmt.name().lexeme();
        return LoxCallable(executeFun, &environment, static_cast<int>(stmt.parameters().size()), functionName);
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
        default:
            throw RuntimeError{ expr.operatr(), "Sorry I cannot do this!" };
        }
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
        return lookupVariable(expr.name(), expr, environment);
    }

    Object evaluateAssignExpr(AssignExpr const& expr, Environment& environment) {
        auto const value = evaluate(expr.value(), environment);
        
        if (auto const it = Lox::locals.find(&expr); it != Lox::locals.end()) {
            environment.assignAt(it->second, expr.name(), value);
        }
        else {
            Lox::globals.assign(expr.name(), value);
        }

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
    Object evaluateGetExpr(GetExpr const& expr, Environment& environment) {
        auto const object = evaluate(expr.object(), environment);
        if (object.isLoxInstance()) {
            return static_cast<LoxInstance>(object).get(expr.name());
        }

        throw RuntimeError{ expr.name(), "Only instances have properties." };
    }
    Object evaluateSetExpr(SetExpr const& expr, Environment& environment) {
        auto const object = evaluate(expr.object(), environment);
        if (!object.isLoxInstance()) {
            throw RuntimeError{ expr.name(), "Only instances have properties." };
        }

        auto const value = evaluate(expr.value(), environment);
        static_cast<LoxInstance>(object).set(expr.name(), value);
        return value;
    }
    Object evaluateThisExpr(ThisExpr const& expr, Environment& environment) {
        return lookupVariable(expr.keyword(), expr, environment);
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
        auto const value = stmt.initializer() ? evaluate(*stmt.initializer(), environment) : Object();
        environment.define(stmt.name().lexeme(), value);
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
        environment.define(stmt.name().lexeme(), loxCallableFromFunctionStmt(stmt, environment));
        return {};
    }

    Object executeReturnStmt(ReturnStmt const& stmt, Environment& environment) {
        auto const value = stmt.value() ? evaluate(*stmt.value(), environment) : Object{};
        throw Return{ value };
    }

    Object executeClassStmt(ClassStmt const& stmt, Environment& environment) {
        environment.define(stmt.name().lexeme(), Object());
        auto methods = std::unordered_map<std::string, LoxCallable>();
        for (auto method : stmt.methods()) {
            methods.insert(std::pair(method->name().lexeme(), loxCallableFromFunctionStmt(*method, environment, stmt.name().lexeme())));
        }
        environment.assign(stmt.name(), LoxClass(stmt.name().lexeme(), methods));
        return {};
    }
    

    // Evaluate function of generic expression:

    template <typename T>
    using EvaluateExprFuncT = std::function<Object(T const&, Environment&)>;

    Object evaluate(Expr const& expr, Environment& environment) {
        static auto const evaluateDispatcher = Dispatcher<Object, Expr const&, Environment&>("evaluate expression",
            EvaluateExprFuncT<BinaryExpr>(evaluateBinaryExpr),
            EvaluateExprFuncT<GroupingExpr>(evaluateGroupingExpr),
            EvaluateExprFuncT<LiteralExpr>(evaluateLiteralExpr),
            EvaluateExprFuncT<UnaryExpr>(evaluateUnaryExpr),
            EvaluateExprFuncT<VariableExpr>(evaluateVariableExpr),
            EvaluateExprFuncT<AssignExpr>(evaluateAssignExpr),
            EvaluateExprFuncT<LogicalExpr>(evaluateLogicalExpr),
            EvaluateExprFuncT<CallExpr>(evaluateCallExpr),
            EvaluateExprFuncT<GetExpr>(evaluateGetExpr),
            EvaluateExprFuncT<SetExpr>(evaluateSetExpr),
            EvaluateExprFuncT<ThisExpr>(evaluateThisExpr)
        );

        return evaluateDispatcher.dispatch(expr, environment);
    }

    // Execute function of generic statement:

    template <typename T>
    using ExecuteStmtFuncT = std::function<Object(T const&, Environment& environment)>;

    Object execute(Stmt const& statement, Environment& environment) {
        static auto const executeDispatcher = Dispatcher<Object, Stmt const&, Environment&>("execute statement",
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

