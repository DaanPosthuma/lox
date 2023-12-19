#include "Interpreter.h"
#include "Stmt.h"
#include "Expr.h"
#include "Lox.h"
#include "Token.h"
#include "TokenType.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace std::string_literals;

namespace {

    auto const identifier = Token(TokenType::IDENTIFIER, "TestClass", Object(), 0);
    auto const dummy = identifier;
    auto const declareClass = ClassStmt(identifier, {});
    auto const variable = VariableExpr(identifier);
    auto const instantiateClassExpr = CallExpr(&variable, dummy, {});
    auto const instantiateClass = ExpressionStmt(&instantiateClassExpr);

    TEST_CASE("Can create instance of a class") {
        REQUIRE(interpret({ &declareClass, &instantiateClass }).isLoxInstance());
        REQUIRE(!Lox::hadError);
    }

}
