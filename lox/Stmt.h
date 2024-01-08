#pragma once
#include "Token.h"
#include "polymorphic.h"
#include <vector>

class Expr;
class VariableExpr;

class Stmt {
public:
    virtual ~Stmt() = default;
};

class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(xyz::polymorphic<Expr> expression);
    Expr const& expression() const { return *mExpression; }

private:
    xyz::polymorphic<Expr> mExpression;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(xyz::polymorphic<Expr> expression);
    Expr const& expression() const { return *mExpression; }

private:
    xyz::polymorphic<Expr> mExpression;
};

class VarStmt : public Stmt {
public:
    VarStmt(Token const& name, std::optional<xyz::polymorphic<Expr>> initializer);
    Token const& name() const { return mName; }
    std::optional<xyz::polymorphic<Expr>> initializer() const { return mInitializer; }

private:
    Token mName;
    std::optional<xyz::polymorphic<Expr>> mInitializer;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<xyz::polymorphic<Stmt>> const& statements);
    std::vector<xyz::polymorphic<Stmt>> const& statements() const { return mStatements; }

private:
    std::vector<xyz::polymorphic<Stmt>> mStatements;
};

class IfStmt : public Stmt {
public:
    IfStmt(xyz::polymorphic<Expr> condition, xyz::polymorphic<Stmt> thenBranch, std::optional<xyz::polymorphic<Stmt>> elseBranch);
    Expr const& condition() const { return *mCondition; }
    Stmt const& thenBranch() const { return *mThenBranch; }
    std::optional<xyz::polymorphic<Stmt>> elseBranch() const { return mElseBranch; }

private:
    xyz::polymorphic<Expr> mCondition;
    xyz::polymorphic<Stmt> mThenBranch;
    std::optional<xyz::polymorphic<Stmt>> mElseBranch;

};

class WhileStmt : public Stmt {
public:
    WhileStmt(xyz::polymorphic<Expr> condition, xyz::polymorphic<Stmt> const& body);
    Expr const& condition() const { return *mCondition; }
    Stmt const& body() const { return *mBody; }

private:
    xyz::polymorphic<Expr> mCondition;
    xyz::polymorphic<Stmt> mBody;

};

class FunctionStmt : public Stmt {
public:
    FunctionStmt(Token const& name, std::vector<Token> const& parameters, BlockStmt const& body);
    
    Token name() const { return mName; }
    std::vector<Token> const& parameters() const { return mParameters; }
    BlockStmt const& body() const { return mBody; }
    
private:
    Token mName;
    std::vector<Token> mParameters;
    BlockStmt mBody;

};

class ReturnStmt : public Stmt {
public:
    ReturnStmt(Token const& keyword, std::optional<xyz::polymorphic<Expr>> value);
    Token const& keyword() const { return mKeyword; }
    std::optional<xyz::polymorphic<Expr>> value() const { return mValue; }

private:
    Token mKeyword;
    std::optional<xyz::polymorphic<Expr>> mValue;
};

class ClassStmt : public Stmt {
public:
    ClassStmt(Token const& name, VariableExpr const* superclass, std::vector<FunctionStmt> const& methods);
    Token const& name() const { return mName; }
    VariableExpr const* superclass() const { return mSuperclass; }
    std::vector<FunctionStmt> const& methods() const { return mMethods; }

private:
    Token mName;
    VariableExpr const* mSuperclass;
    std::vector<FunctionStmt> mMethods;
};
