#include "Token.h"
#include "TokenType.h"
#include "Object.h"
#include "Stmt.h"
#include "Expr.h"
#include "Lox.h"
#include "Resolver.h"
#include "TestGuard.h"
#include "polymorphic.h"
#include <catch2/catch_test_macros.hpp>

using namespace std::string_literals;

namespace {

    auto const identifier = Token(TokenType::IDENTIFIER, "test", Object(), 0);
    auto const declareVariable = xyz::make_polymorphic<Stmt>(VarStmt(identifier, std::nullopt));
    auto const variableExpr = xyz::make_polymorphic<Expr>(VariableExpr(identifier));
    auto const useVariable = xyz::make_polymorphic<Stmt>(ExpressionStmt(variableExpr));

    auto const literalExpr = xyz::make_polymorphic<Expr>(LiteralExpr(Object("test"s)));
    auto const assignExpr = xyz::make_polymorphic<Expr>(AssignExpr(identifier, literalExpr));
    auto const assignStmt = xyz::make_polymorphic<Stmt>(ExpressionStmt(assignExpr));

    TEST_CASE("Declaration does not procude locals") {
        TestGuard guard;
        auto const block = xyz::make_polymorphic<Stmt>(BlockStmt({ declareVariable }));
        resolve({ block });
        REQUIRE(!Lox::hadError);
    }

    TEST_CASE("Using a global variable does not produce any locals") {
        TestGuard guard;
        resolve({ declareVariable, useVariable });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals .empty());
    }

    TEST_CASE("Using a variable in block procudes a resolved local") {
        TestGuard guard;
        auto const block = xyz::make_polymorphic<Stmt>(BlockStmt({ declareVariable, useVariable }));
        resolve({ block });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals.contains(variableExpr));
    }

    TEST_CASE("Assigning a varable produces local") {
        TestGuard guard;
        auto const block = xyz::make_polymorphic<Stmt>(BlockStmt({ declareVariable, assignStmt }));
        resolve({ block });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals.contains(assignExpr));
    }

}
