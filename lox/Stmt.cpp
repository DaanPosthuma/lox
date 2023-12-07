#include "Stmt.h"
#include <cassert>

ExpressionStmt::ExpressionStmt(Expr const* expression) : mExpression(expression) {
    assert(expression && "ExpressionStmt ctor: expression cannot be nullptr");
}

PrintStmt::PrintStmt(Expr const* expression) : mExpression(expression) {
    assert(expression && "PrintStmt ctor: expression cannot be nullptr");
}

VarStmt::VarStmt(Token const& token, Expr const* initializer) : mToken(token), mInitializer(initializer) {
    assert(initializer && "VarStmt ctor: initializer cannot be nullptr");
}

BlockStmt::BlockStmt(std::vector<Stmt*> const& statements) : mStatements(statements) {
}

IfStmt::IfStmt(Expr const* condition, Stmt* const& thenBranch, Stmt* const& elseBranch) : mCondition(condition), mThenBranch(thenBranch), mElseBranch(elseBranch) {
    assert(condition && "IfStmt ctor: condition cannot be null");
    assert(condition && "IfStmt ctor: then branch cannot be null");
}

WhileStmt::WhileStmt(Expr const* condition, Stmt* const& body) : mCondition(condition), mBody(body) {
    assert(condition && "WhileStmt ctor: condition cannot be null");
    assert(condition && "WhileStmt ctor: body cannot be null");
}

FunctionStmt::FunctionStmt(Token const& name, std::vector<Token> const& parameters, BlockStmt const& body) : mName(name), mParameters(parameters), mBody(body) {
}
