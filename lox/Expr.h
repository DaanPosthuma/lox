#pragma once

#include "Token.h"

class Expr {
public:
    Expr() = default;
    Expr(Expr const&) = delete;
    Expr& operator=(Expr const&) = delete;

    virtual ~Expr() = default;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr* const left, Token const& operatr, Expr* const right);

    Expr const& getLeft() const { return *mLeft; }
    Token const& getOperator() const { return mOperator; }
    Expr const& getRight() const { return *mRight; }

private:
    Expr* mLeft;
    Token mOperator;
    Expr* mRight;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token const& operatr, Expr* const right);
    
    Token const& getOperator() const { return mOperator; }
    Expr const& getRight() const { return *mRight; }

private:
    Token mOperator;
    Expr* mRight;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(Expr* const expression);

    Expr const& getExpression() const { return *mExpression; }

private:
    Expr* mExpression;
};

class LiteralExpr : public Expr {
public:
    LiteralExpr(Object const& value);

    Object const& getValue() const { return mValue; }

private:
    Object mValue;
};
