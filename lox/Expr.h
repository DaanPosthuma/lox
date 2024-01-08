#pragma once

#include "Token.h"
#include "polymorphic.h"
#include <vector>

//#define ADDEQUALITY(CONCRETETYPE) std::partial_ordering operator<=>(CONCRETETYPE const& other) const = default;\
//bool IsEqual(Expr const& rhs) const override { return this->IsEqualImpl(rhs); }

#define ADDEQUALITY(CONCRETETYPE) bool IsEqual(Expr const& rhs) const override { return false; }

class Expr {
public:
    Expr() {
        static int exprid = 0;
        mExprId = exprid++;
    }
    virtual ~Expr() = default;
    std::partial_ordering operator<=>(Expr const& other) const = default;

    virtual bool IsEqual(Expr const& rhs) const = 0;

    //friend bool operator==(xyz::polymorphic<Expr> const& lhs, xyz::polymorphic<Expr> const& rhs);
    
protected:
    //template <class ConcreteT>
    //bool IsEqualImpl(this ConcreteT const& lhs, Expr const& rhs) {
    //    if (auto concreteRhs = dynamic_cast<ConcreteT const*>(&rhs)) {
    //        return lhs == *concreteRhs;
    //    }
    //    return false;
    //}
    int mExprId;
};

inline bool operator==(xyz::polymorphic<Expr> const& lhs, xyz::polymorphic<Expr> const& rhs) {
    return lhs->IsEqual(*rhs);
}

class BinaryExpr : public Expr {
public:
    BinaryExpr(xyz::polymorphic<Expr> left, Token const& operatr, xyz::polymorphic<Expr> right);

    Expr const& left() const { return *mLeft; }
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

    ADDEQUALITY(BinaryExpr)

private:
    xyz::polymorphic<Expr> mLeft;
    Token mOperator;
    xyz::polymorphic<Expr> mRight;
};

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token const& operatr, xyz::polymorphic<Expr> right);
    
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

    ADDEQUALITY(UnaryExpr)

private:
    Token mOperator;
    xyz::polymorphic<Expr> mRight;
};

class GroupingExpr : public Expr {
public:
    GroupingExpr(xyz::polymorphic<Expr> expression);

    Expr const& expression() const { return *mExpression; }

    ADDEQUALITY(GroupingExpr)

private:
    xyz::polymorphic<Expr> mExpression;
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
    AssignExpr(Token const& name, xyz::polymorphic<Expr> value);

    Token const& name() const { return mName; }
    Expr const& value() const { return *mValue; }

    ADDEQUALITY(AssignExpr)

private:
    Token mName;
    xyz::polymorphic<Expr> mValue;
};

class LogicalExpr : public Expr {
public:
    LogicalExpr(xyz::polymorphic<Expr> left, Token const& operatr, xyz::polymorphic<Expr> right);

    Expr const& left() const { return *mLeft; }
    Token const& operatr() const { return mOperator; }
    Expr const& right() const { return *mRight; }

    ADDEQUALITY(LogicalExpr)

private:
    xyz::polymorphic<Expr> mLeft;
    Token mOperator;
    xyz::polymorphic<Expr> mRight;
};

class CallExpr : public Expr {
public:
    CallExpr(xyz::polymorphic<Expr> callee, Token const& paren, std::vector<xyz::polymorphic<Expr>> const& arguments);

    Expr const& callee() const { return *mCallee; }
    Token const& paren() const { return mParen; }
    std::vector<xyz::polymorphic<Expr>> const& arguments() const { return mArguments; }

    ADDEQUALITY(CallExpr)

private:
    xyz::polymorphic<Expr> mCallee;
    Token mParen;
    std::vector<xyz::polymorphic<Expr>> mArguments;
};

class GetExpr : public Expr {
public:
    GetExpr(xyz::polymorphic<Expr> object, Token const& name);
    xyz::polymorphic<Expr> const& object() const { return mObject; }
    Token const& name() const { return mName; }

    ADDEQUALITY(GetExpr)

private:
    xyz::polymorphic<Expr> mObject;
    Token mName;
};

class SetExpr : public Expr {
public:
    SetExpr(xyz::polymorphic<Expr> object, Token const& name, xyz::polymorphic<Expr> value);
    Expr const& object() const { return *mObject; }
    Token const& name() const { return mName; }
    Expr const& value() const { return *mValue; }

    ADDEQUALITY(SetExpr)

private:
    xyz::polymorphic<Expr> mObject;
    Token mName;
    xyz::polymorphic<Expr> mValue;
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
