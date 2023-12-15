#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"
#include "Lox.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

namespace {

    TEST_CASE("Scanner produces tokens") {
        REQUIRE(scanTokens("").size() == 1);
        REQUIRE(!Lox::hadError);

        REQUIRE(scanTokens(";").size() == 2);
        REQUIRE(!Lox::hadError);

        REQUIRE(scanTokens("fun(n) { print n; } fun();").size() == 14);
        REQUIRE(!Lox::hadError);
    }

}
