#include "Resolver.h"
#include "Dispatcher.h"
#include "Expr.h"
#include "Stmt.h"
#include "Lox.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace {

    enum class FunctionType {
        NONE, FUNCTION
    };

    using Scopes = std::vector<std::unordered_map<std::string, bool>>;

    struct ResolverContext {
        Scopes scopes;
        ResolvedLocals locals;
        FunctionType currentFunction = FunctionType::NONE;
    };

    void resolve(Stmt const& stmt, ResolverContext& context);
    void resolve(Expr const& expr, ResolverContext& context);

    void beginScope(Scopes& scopes) {
        scopes.emplace_back();
    }
    void endScope(Scopes& scopes) {
        scopes.pop_back();
    }
    void declare(Token const& name, Scopes& scopes) {
        if (scopes.empty()) return;
        if (scopes.back().contains(name.lexeme())) Lox::error(name, "Already a variable with this name in this scope.");
        scopes.back()[name.lexeme()] = false;
    }
    void define(Token const& name, Scopes& scopes) {
        if (scopes.empty()) return;
        scopes.back()[name.lexeme()] = true;
    }
    void resolveLocal(Expr const& expr, Token const& name, ResolverContext& context) {
        for (auto i = static_cast<int>(context.scopes.size()) - 1; i >= 0; --i) {
            if (context.scopes[i].contains(name.lexeme())) {
                context.locals[&expr] = static_cast<int>(context.scopes.size()) - 1 - i;
                return;
            }
        }
    }
    void resolveFunction(FunctionStmt const& stmt, FunctionType type, ResolverContext& context) {
        auto const enclosingFunction = std::exchange(context.currentFunction, type);

        beginScope(context.scopes);
        for (Token const& param : stmt.parameters()) {
            declare(param, context.scopes);
            define(param, context.scopes);
        }
        resolve(stmt.body(), context);
        endScope(context.scopes);

        context.currentFunction = enclosingFunction;
    }

    // Statements:
    void resolveVarStmt(VarStmt const& stmt, ResolverContext& context) {
        declare(stmt.name(), context.scopes);
        if (stmt.initializer()) {
            resolve(*stmt.initializer(), context);
        }
        define(stmt.name(), context.scopes);
    }
    void resolveBlockStmt(BlockStmt const& stmt, ResolverContext& context) {
        beginScope(context.scopes);
        for (auto* stmt : stmt.statements()) {
            resolve(*stmt, context);
        }
        endScope(context.scopes);
    }
    void resolveFunctionStmt(FunctionStmt const& stmt, ResolverContext& context) {
        declare(stmt.name(), context.scopes);
        define(stmt.name(), context.scopes);
        resolveFunction(stmt, FunctionType::FUNCTION, context);
    }
    void resolveExpressionStmt(ExpressionStmt const& stmt, ResolverContext& context) {
        resolve(stmt.expression(), context);
    }
    void resolveIfStmt(IfStmt const& stmt, ResolverContext& context) {
        resolve(stmt.condition(), context);
        resolve(stmt.thenBranch(), context);
        if (stmt.elseBranch()) resolve(*stmt.elseBranch(), context);
    }
    void resolvePrintStmt(PrintStmt const& stmt, ResolverContext& context) {
        resolve(stmt.expression(), context);
    }
    void resolveWhileStmt(WhileStmt const& stmt, ResolverContext& context) {
        resolve(stmt.condition(), context);
        resolve(stmt.body(), context);
    }
    void resolveReturnStmt(ReturnStmt const& stmt, ResolverContext& context) {
        if (context.currentFunction == FunctionType::NONE) {
            Lox::error(stmt.keyword(), "Can't return from top-level code.");
        }
        if (stmt.value()) resolve(*stmt.value(), context);
    }

    // Expressions:
    void resolveVariableExpr(VariableExpr const& expr, ResolverContext& context) {
        auto const& key = expr.name().lexeme();
        auto const& scopes = context.scopes;
        if (!scopes.empty() && scopes.back().contains(key) && !scopes.back().at(key))
        {
            Lox::error(expr.name(), "Can't read local variable in its own initializer.");
        }
        resolveLocal(expr, expr.name(), context);
    }
    void resolveBinaryExpr(BinaryExpr const& expr, ResolverContext& context) {
        resolve(expr.left(), context);
        resolve(expr.right(), context);
    }
    void resolveGroupingExpr(GroupingExpr const& expr, ResolverContext& context) {
        resolve(expr.expression(), context);
    }
    void resolveLiteralExpr(LiteralExpr const& expr, ResolverContext& context) {
    }
    void resolveUnaryExpr(UnaryExpr const& expr, ResolverContext& context) {
        resolve(expr.right(), context);
    }
    void resolveAssignExpr(AssignExpr const& expr, ResolverContext& context) {
        resolve(expr.value(), context);
        resolveLocal(expr, expr.name(), context);
    }
    void resolveLogicalExpr(LogicalExpr const& expr, ResolverContext& context) {
        resolve(expr.left(), context);
        resolve(expr.right(), context);
    }
    void resolveCallExpr(CallExpr const& expr, ResolverContext& context) {
        resolve(expr.callee(), context);
        for (auto const* arg : expr.arguments()) {
            resolve(*arg, context);
        }
    }

    template <typename T>
    using ResolveStmtFuncT = std::function<void(T const&, ResolverContext&)>;

    void resolve(Stmt const& stmt, ResolverContext& context) {
        static auto const resolveDispatcher = Dispatcher<void, Stmt const&, ResolverContext&>(
            ResolveStmtFuncT<ExpressionStmt>(resolveExpressionStmt),
            ResolveStmtFuncT<IfStmt>(resolveIfStmt),
            ResolveStmtFuncT<PrintStmt>(resolvePrintStmt),
            ResolveStmtFuncT<WhileStmt>(resolveWhileStmt),
            ResolveStmtFuncT<VarStmt>(resolveVarStmt),
            ResolveStmtFuncT<BlockStmt>(resolveBlockStmt),
            ResolveStmtFuncT<FunctionStmt>(resolveFunctionStmt),
            ResolveStmtFuncT<ReturnStmt>(resolveReturnStmt)
        );

        resolveDispatcher.dispatch(stmt, context);
    }
    
    template <typename T>
    using ResolveExprFuncT = std::function<void(T const&, ResolverContext&)>;

    void resolve(Expr const& expr, ResolverContext& context) {
        static auto const resolveDispatcher = Dispatcher<void, Expr const&, ResolverContext&>(
            ResolveExprFuncT<BinaryExpr>(resolveBinaryExpr),
            ResolveExprFuncT<GroupingExpr>(resolveGroupingExpr),
            ResolveExprFuncT<LiteralExpr>(resolveLiteralExpr),
            ResolveExprFuncT<UnaryExpr>(resolveUnaryExpr),
            ResolveExprFuncT<VariableExpr>(resolveVariableExpr),
            ResolveExprFuncT<AssignExpr>(resolveAssignExpr),
            ResolveExprFuncT<LogicalExpr>(resolveLogicalExpr),
            ResolveExprFuncT<CallExpr>(resolveCallExpr)
        );

        resolveDispatcher.dispatch(expr, context);
    }
}

ResolvedLocals resolve(std::vector<Stmt const*> const& statements) {
    ResolverContext context;
    for (auto* stmt : statements) {
        resolve(*stmt, context);
    }
    return context.locals;
}
