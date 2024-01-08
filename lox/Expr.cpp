#include "Expr.h"
#include <cassert>

BinaryExpr::BinaryExpr(xyz::polymorphic<Expr> left, Token const& operatr, xyz::polymorphic<Expr> right) : mLeft(left), mOperator(operatr), mRight(right) {}

UnaryExpr::UnaryExpr(Token const& operatr, xyz::polymorphic<Expr> right) : mOperator(operatr), mRight(right) {}

GroupingExpr::GroupingExpr(xyz::polymorphic<Expr> expression) : mExpression(expression) {}

LiteralExpr::LiteralExpr(Object const& value) : mValue(value) {}

VariableExpr::VariableExpr(Token const& name) : mName(name) {}

AssignExpr::AssignExpr(Token const& name, xyz::polymorphic<Expr> value) : mName(name), mValue(value) {}

LogicalExpr::LogicalExpr(xyz::polymorphic<Expr> left, Token const& operatr, xyz::polymorphic<Expr> right) : mLeft(left), mOperator(operatr), mRight(right) {}

CallExpr::CallExpr(xyz::polymorphic<Expr> callee, Token const& paren, std::vector<xyz::polymorphic<Expr>> const& arguments) : mCallee(callee), mParen(paren), mArguments(arguments) {}

GetExpr::GetExpr(xyz::polymorphic<Expr> object, Token const& name) : mObject(object), mName(name) {}

SetExpr::SetExpr(xyz::polymorphic<Expr> object, Token const& name, xyz::polymorphic<Expr> value) : mObject(object), mName(name), mValue(value) {}

ThisExpr::ThisExpr(Token const& keyword) : mKeyword(keyword) {}

SuperExpr::SuperExpr(Token const& keyword, Token const& method) : mKeyword(keyword), mMethod(method) {}
