#include "Stmt.h"
#include <cassert>

ExpressionStmt::ExpressionStmt(Expr* expression) : mExpression(expression) {
    assert(expression && "ExpressionStmt ctor: expression cannot be nullptr");
}

PrintStmt::PrintStmt(Expr* expression) : mExpression(expression) {
    assert(expression && "PrintStmt ctor: expression cannot be nullptr");
}

VarStmt::VarStmt(Token const& token, Expr* initializer) : mToken(token), mInitializer(initializer) {
    assert(initializer && "VarStmt ctor: initializer cannot be nullptr");
}
