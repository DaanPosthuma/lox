#include "Stmt.h"
#include "Expr.h"
#include <cassert>

ExpressionStmt::ExpressionStmt(xyz::polymorphic<Expr> expression) : mExpression(expression) {}

PrintStmt::PrintStmt(xyz::polymorphic<Expr> expression) : mExpression(expression) {}

VarStmt::VarStmt(Token const& name, std::optional<xyz::polymorphic<Expr>> initializer) : mName(name), mInitializer(initializer) {}

BlockStmt::BlockStmt(std::vector<xyz::polymorphic<Stmt>> const& statements) : mStatements(statements) {}

IfStmt::IfStmt(xyz::polymorphic<Expr> condition, xyz::polymorphic<Stmt> thenBranch, std::optional<xyz::polymorphic<Stmt>> elseBranch) : mCondition(condition), mThenBranch(thenBranch), mElseBranch(elseBranch) {}

WhileStmt::WhileStmt(xyz::polymorphic<Expr> condition, xyz::polymorphic<Stmt> const& body) : mCondition(condition), mBody(body) {}

FunctionStmt::FunctionStmt(Token const& name, std::vector<Token> const& parameters, BlockStmt const& body) : mName(name), mParameters(parameters), mBody(body) {}

ReturnStmt::ReturnStmt(Token const& keyword, std::optional<xyz::polymorphic<Expr>> value) : mKeyword(keyword), mValue(value) {}

ClassStmt::ClassStmt(Token const& name, VariableExpr const* superclass, std::vector<FunctionStmt> const& methods) : mName(name), mSuperclass(superclass), mMethods(methods) {}
