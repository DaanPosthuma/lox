#include "Token.h"
#include "TokenType.h"
#include "Object.h"
#include "Stmt.h"
#include "Expr.h"
#include "Lox.h"
#include "Resolver.h"
#include <catch2/catch_test_macros.hpp>

using namespace std::string_literals;

namespace {

    auto const identifier = Token(TokenType::IDENTIFIER, "test", Object(), 0);
    auto const declareVariable = VarStmt(identifier, nullptr);
    auto const variableExpr = VariableExpr(identifier);
    auto const useVariable = ExpressionStmt(&variableExpr);

    auto const literalExpr = LiteralExpr(Object("test"s));
    auto const assignExpr = AssignExpr(identifier, &literalExpr);
    auto const assignStmt = ExpressionStmt(&assignExpr);

    TEST_CASE("Declaration does not procude locals") {
        auto const block = BlockStmt({ &declareVariable });
        auto const resolvedLocals = resolve({ &block });
        REQUIRE(!Lox::hadError);
        REQUIRE(resolvedLocals.empty());

    }

    TEST_CASE("Using a global variable does not produce any locals") {
        auto const resolvedLocals = resolve({ &declareVariable, &useVariable });
        REQUIRE(!Lox::hadError);
        REQUIRE(resolvedLocals.empty());

    }

    TEST_CASE("Using a variable in block procudes a resolved local") {
        auto const block = BlockStmt({ &declareVariable, &useVariable });
        auto const resolvedLocals = resolve({ &block });
        REQUIRE(!Lox::hadError);
        REQUIRE(resolvedLocals.contains(&variableExpr));

    }

    TEST_CASE("Assigning a varable produces local") {
        auto const block = BlockStmt({ &declareVariable, &assignStmt });
        auto const resolvedLocals = resolve({ &block });
        REQUIRE(!Lox::hadError);
        REQUIRE(resolvedLocals.contains(&assignExpr));

    }

}
