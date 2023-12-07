#pragma once
#include "Token.h"
#include <vector>

class Expr;


class Stmt {
public:
    virtual ~Stmt() = default;
};

class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(Expr const* expression);
    Expr const& expression() const { return *mExpression; }

private:
    Expr const* mExpression;
};

class PrintStmt : public Stmt {
public:
    PrintStmt(Expr const* expression);
    Expr const& expression() const { return *mExpression; }

private:
    Expr const* mExpression;
};

class VarStmt : public Stmt {
public:
    VarStmt(Token const& token, Expr const* initializer);
    Token const& token() const { return mToken; }
    Expr const& initializer() const { return *mInitializer; }

private:
    Token mToken;
    Expr const* mInitializer;
};

class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<Stmt*> const& statements);
    std::vector<Stmt*> const& statements() const { return mStatements; }

private:
    std::vector<Stmt*> mStatements;
};

class IfStmt : public Stmt {
public:
    IfStmt(Expr const* condition, Stmt* const& thenBranch, Stmt* const& elseBranch);
    Expr const& condition() const { return *mCondition; }
    Stmt const& thenBranch() const { return *mThenBranch; }
    Stmt const* elseBranch() const { return mElseBranch; }

private:
    Expr const* mCondition;
    Stmt* mThenBranch;
    Stmt* mElseBranch;

};

class WhileStmt : public Stmt {
public:
    WhileStmt(Expr const* condition, Stmt* const& body);
    Expr const& condition() const { return *mCondition; }
    Stmt const& body() const { return *mBody; }

private:
    Expr const* mCondition;
    Stmt* mBody;

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