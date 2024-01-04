#pragma once

#include "Token.h"
#include <vector>

#define ADDEQUALITY(CONCRETETYPE) std::partial_ordering operator<=>(CONCRETETYPE const& other) const = default;\
bool IsEqual(Expr const& rhs) const override { return this->IsEqualImpl(rhs); }

class Expr {
public:
    Expr() {
        static int exprid = 0;
        mExprId = exprid++;
    }
    virtual ~Expr() = default;
    std::partial_ordering operator<=>(Expr const& other) const = default;

    virtual bool IsEqual(Expr const& rhs) const = 0;
    
protected:
    template <class ConcreteT>
    bool IsEqualImpl(this ConcreteT const& lhs, Expr const& rhs) {
        if (auto concreteRhs = dynamic_cast<ConcreteT const*>(&rhs)) {
            return lhs == *concreteRhs;
        }
        return false;
    }
    int mExprId;
};

class BinaryExpr : public Expr {
public:
    BinaryExpr(Expr const* left, Token const& operatr, Expr const* right);

    Expr const& left() const { return *mLeft; }
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

    ADDEQUALITY(BinaryExpr)

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

    ADDEQUALITY(UnaryExpr)

private:
    Token mOperator;
    Expr const* mRight;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(Expr const* expression);

    Expr const& expression() const { return *mExpression; }

    ADDEQUALITY(GroupingExpr)

private:
    Expr const* mExpression;
};

class LiteralExpr : public Expr {
public:
    LiteralExpr(Object const& value);

    Object const& value() const { return mValue; }

    ADDEQUALITY(LiteralExpr)

private:
    Object mValue;
};

class VariableExpr : public Expr {
public:
    VariableExpr(Token const& name);

    Token const& name() const { return mName; }

    ADDEQUALITY(VariableExpr)

private:
    Token mName;
};

class AssignExpr : public Expr {
public:
    AssignExpr(Token const& name, Expr const* value);

    Token const& name() const { return mName; }
    Expr const& value() const { return *mValue; }

    ADDEQUALITY(AssignExpr)

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

    ADDEQUALITY(LogicalExpr)

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

    ADDEQUALITY(CallExpr)

private:
    Expr const* mCallee;
    Token mParen;
    std::vector<Expr const*> mArguments;
};

class GetExpr : public Expr {
public:
    GetExpr(Expr const* object, Token const& name);
    Expr const& object() const { return *mObject; }
    Token const& name() const { return mName; }

    ADDEQUALITY(GetExpr)

private:
    Expr const* mObject;
    Token mName;
};

class SetExpr : public Expr {
public:
    SetExpr(Expr const* object, Token const& name, Expr const* value);
    Expr const& object() const { return *mObject; }
    Token const& name() const { return mName; }
    Expr const& value() const { return *mValue; }

    ADDEQUALITY(SetExpr)

private:
    Expr const* mObject;
    Token mName;
    Expr const* mValue;
};

class ThisExpr : public Expr {
public:
    ThisExpr(Token const& keyword);

    Token const& keyword() const { return mKeyword; }

    ADDEQUALITY(ThisExpr)

private:
    Token mKeyword;
};

class SuperExpr : public Expr {
public:
    SuperExpr(Token const& keyword, Token const& method);

    Token const& keyword() const { return mKeyword; }
    Token const& method() const { return mMethod; }

    ADDEQUALITY(SuperExpr)

private:
    Token mKeyword;
    Token mMethod;
};
