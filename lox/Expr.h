#pragma once

#include "Token.h"
#include <vector>

class Expr {
public:
    virtual ~Expr() = default;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr const* left, Token const& operatr, Expr const* right);

    Expr const& left() const { return *mLeft; }
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

private:
    Expr const* mLeft;
    Token mOperator;
    Expr const* mRight;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token const& operatr, Expr const* right);
    
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

private:
    Token mOperator;
    Expr const* mRight;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(Expr const* expression);

    Expr const& expression() const { return *mExpression; }

private:
    Expr const* mExpression;
};

class LiteralExpr : public Expr {
public:
    LiteralExpr(Object const& value);

    Object const& value() const { return mValue; }

private:
    Object mValue;
};

class VariableExpr : public Expr {
public:
    VariableExpr(Token const& name);

    Token const& name() const { return mName; }

private:
    Token mName;
};

class AssignExpr : public Expr {
public:
    AssignExpr(Token const& name, Expr const* value);

    Token const& name() const { return mName; }
    Expr const& value() const { return *mValue; }

private:
    Token mName;
    Expr const* mValue;
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(Expr const* left, Token const& operatr, Expr const* right);

    Expr const& left() const { return *mLeft; }
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

private:
    Expr const* mLeft;
    Token mOperator;
    Expr const* mRight;
};

class CallExpr : public Expr {
public:
    CallExpr(Expr const* callee, Token const& paren, std::vector<Expr const*> const& arguments);

    Expr const& callee() const { return *mCallee; }
    Token const& paren() const { return mParen; }
    std::vector<Expr const*> const& arguments() const { return mArguments; }

private:
    Expr const* mCallee;
    Token mParen;
    std::vector<Expr const*> mArguments;
};
