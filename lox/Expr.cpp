#include "Expr.h"
#include <cassert>

BinaryExpr::BinaryExpr(Expr* const left, Token const& operatr, Expr* const right) : mLeft(left), mOperator(operatr), mRight(right) {
    assert(left && "BinaryExpr ctor: left cannot be nullptr");
    assert(right && "BinaryExpr ctor: right cannot be nullptr");
}

UnaryExpr::UnaryExpr(Token const& operatr, Expr* const right) : mOperator(operatr), mRight(right) {
    assert(right && "UnaryExpr ctor: right cannot be nullptr");
}

GroupingExpr::GroupingExpr(Expr* const expression) : mExpression(expression) {
    assert(expression && "GroupingExpr ctor: expression cannot be nullptr");
}

LiteralExpr::LiteralExpr(Object const& value) : mValue(value) {}
