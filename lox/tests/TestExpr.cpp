#include "Token.h"
#include "TokenType.h"
#include "Expr.h"
#include <catch2/catch_test_macros.hpp>

namespace {

    TEST_CASE("Expr") {
        auto a = ThisExpr(Token(TokenType::VAR, "", Object(), 0));
        auto b = ThisExpr(Token(TokenType::ENABLE_DEBUG, "", Object(), 0));
        auto sameAsA = ThisExpr(Token(TokenType::VAR, "", Object(), 0));
        
        REQUIRE(a == a);
        REQUIRE(b == b);
        REQUIRE(a != b);
        REQUIRE(b != a);
        
        // expressions contain unique id, so even though they are logically the same they are not considered equal. 
        // This is a requirement of the resolver, otherwise variables in different scopes would get mixed up
        REQUIRE(a != sameAsA); 

        REQUIRE(a.IsEqual(a));
        REQUIRE(b.IsEqual(b));
        REQUIRE(!a.IsEqual(b));
        REQUIRE(!b.IsEqual(a));
        REQUIRE(!a.IsEqual(sameAsA));
        REQUIRE(!sameAsA.IsEqual(a));
    }

}
