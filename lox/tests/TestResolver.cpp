#include "Token.h"
#include "TokenType.h"
#include "Object.h"
#include "Stmt.h"
#include "Expr.h"
#include "Lox.h"
#include "Resolver.h"
#include "ResetState.h"
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
        ResetState guard;
        auto const block = BlockStmt({ &declareVariable });
        resolve({ &block });
        REQUIRE(!Lox::hadError);
    }

    TEST_CASE("Using a global variable does not produce any locals") {
        ResetState guard;
        resolve({ &declareVariable, &useVariable });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals .empty());
    }

    TEST_CASE("Using a variable in block procudes a resolved local") {
        ResetState guard;
        auto const block = BlockStmt({ &declareVariable, &useVariable });
        resolve({ &block });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals.contains(&variableExpr));        
    }

    TEST_CASE("Assigning a varable produces local") {
        ResetState guard;
        auto const block = BlockStmt({ &declareVariable, &assignStmt });
        resolve({ &block });
        REQUIRE(!Lox::hadError);
        REQUIRE(Lox::locals.contains(&assignExpr));
    }

}
