#include "Stmt.h"
#include <cassert>

ExpressionStmt::ExpressionStmt(Expr* expression) : mExpression(expression) {
    assert(expression && "ExpressionStmt ctor: expression cannot be nullptr");
}

PrintStmt::PrintStmt(Expr* expression) : mExpression(expression) {
    assert(expression && "PrintStmt ctor: expression cannot be nullptr");
}

VarStmt::VarStmt(Token const& token, Expr* initializer) : mToken(token), mInitializer(initializer) {
    assert(initializer && "VarStmt ctor: initializer cannot be nullptr");
}

BlockStmt::BlockStmt(std::vector<Stmt*> const& statements) : mStatements(statements) {
}

IfStmt::IfStmt(Expr* condition, Stmt* const& thenBranch, Stmt* const& elseBranch) : mCondition(condition), mThenBranch(thenBranch), mElseBranch(elseBranch) {
    assert(condition && "IfStmt ctor: condition cannot be null");
    assert(condition && "IfStmt ctor: then branch cannot be null");
}

WhileStmt::WhileStmt(Expr* condition, Stmt* const& body) : mCondition(condition), mBody(body) {
    assert(condition && "WhileStmt ctor: condition cannot be null");
    assert(condition && "WhileStmt ctor: body cannot be null");
}
