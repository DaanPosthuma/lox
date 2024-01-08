#include "Interpreter.h"
#include "Stmt.h"
#include "Expr.h"
#include "Lox.h"
#include "Token.h"
#include "TokenType.h"
#include "polymorphic.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace std::string_literals;

namespace {

    auto const identifier = Token(TokenType::IDENTIFIER, "TestClass", Object(), 0);
    auto const dummy = identifier;
    auto const declareClass = xyz::make_polymorphic<Stmt>(ClassStmt(identifier, nullptr, {}));
    auto const variable = xyz::make_polymorphic<Expr>(VariableExpr(identifier));
    auto const instantiateClassExpr = xyz::make_polymorphic<Expr>(CallExpr(variable, dummy, {}));
    auto const instantiateClass = xyz::make_polymorphic<Stmt>(ExpressionStmt(instantiateClassExpr));

    TEST_CASE("Can create instance of a class") {
        REQUIRE(interpret({ declareClass, instantiateClass }).isLoxInstance());
        REQUIRE(!Lox::hadError);
    }

}
