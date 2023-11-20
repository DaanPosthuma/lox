#pragma once
#include "Token.h"
#include <vector>

class Expr;


class Stmt {
public:
    Stmt() = default;
    Stmt(Stmt const&) = delete;
    Stmt& operator=(Stmt const&) = delete;

    virtual ~Stmt() = default;
};

class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(Expr* expression);
    Expr const& getExpression() const { return *mExpression; }

private:
    Expr* mExpression;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(Expr* expression);
    Expr const& getExpression() const { return *mExpression; }

private:
    Expr* mExpression;
};

class VarStmt : public Stmt {
public:
    VarStmt(Token const& token, Expr* initializer);
    Token const& getToken() const { return mToken; }
    Expr const& getInitializer() const { return *mInitializer; }

private:
    Token mToken;
    Expr* mInitializer;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<Stmt*> const& statements);
    std::vector<Stmt*> const& getStatements() const { return mStatements; }

private:
    std::vector<Stmt*> mStatements;
};

class IfStmt : public Stmt {
public:
    IfStmt(Expr* condition, Stmt* const& thenBranch, Stmt* const& elseBranch);
    Expr const& getCondition() const { return *mCondition; }
    Stmt const& getThenBranch() const { return *mThenBranch; }
    Stmt const* getElseBranch() const { return mElseBranch; }

private:
    Expr* mCondition;
    Stmt* mThenBranch;
    Stmt* mElseBranch;

};

class WhileStmt : public Stmt {
public:
    WhileStmt(Expr* condition, Stmt* const& body);
    Expr const& getCondition() const { return *mCondition; }
    Stmt const& getBody() const { return *mBody; }

private:
    Expr* mCondition;
    Stmt* mBody;

};
