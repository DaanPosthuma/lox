#include "Expr.h"
#include <cassert>

BinaryExpr::BinaryExpr(Expr const* left, Token const& operatr, Expr const* right) : mLeft(left), mOperator(operatr), mRight(right) {
    assert(left && "BinaryExpr ctor: left cannot be nullptr");
    assert(right && "BinaryExpr ctor: right cannot be nullptr");
}

UnaryExpr::UnaryExpr(Token const& operatr, Expr const* right) : mOperator(operatr), mRight(right) {
    assert(right && "UnaryExpr ctor: right cannot be nullptr");
}

GroupingExpr::GroupingExpr(Expr const* expression) : mExpression(expression) {
    assert(expression && "GroupingExpr ctor: expression cannot be nullptr");
}

LiteralExpr::LiteralExpr(Object const& value) : mValue(value) {}

VariableExpr::VariableExpr(Token const& name) : mName(name) {}

AssignExpr::AssignExpr(Token const& name, Expr const* value) : mName(name), mValue(value) {
    assert(value && "AssignExpr ctor: value cannot be nullptr");
}

LogicalExpr::LogicalExpr(Expr const* left, Token const& operatr, Expr const* right) : mLeft(left), mOperator(operatr), mRight(right) {
    assert(left && "LogicalExpr ctor: left cannot be nullptr");
    assert(right && "LogicalExpr ctor: right cannot be nullptr");
}

CallExpr::CallExpr(Expr const* callee, Token const& paren, std::vector<Expr const*> const& arguments) : mCallee(callee), mParen(paren), mArguments(arguments) {
    assert(callee && "CallExpr ctor: callee cannot be nullptr");
    for (auto argument : arguments) {
        assert(argument && "CallExpr ctor: argument cannot be nullptr");
    }
}

GetExpr::GetExpr(Expr const* object, Token const& name) : mObject(object), mName(name) {
    assert(object && "GetExpr ctor: object cannot be nullptr");
}

SetExpr::SetExpr(Expr const* object, Token const& name, Expr const* value) : mObject(object), mName(name), mValue(value) {
    assert(object && "SetExpr ctor: object cannot be nullptr");
    assert(value && "SetExpr ctor: value cannot be nullptr");
}

ThisExpr::ThisExpr(Token const& keyword) : mKeyword(keyword) {
}

SuperExpr::SuperExpr(Token const& keyword, Token const& method) : mKeyword(keyword), mMethod(method) {
}
