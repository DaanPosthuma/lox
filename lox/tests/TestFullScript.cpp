#include "Scanner.h"
#include "Parser.h"
#include "Resolver.h"
#include "Interpreter.h"
#include "Object.h"
#include "Token.h"
#include "Lox.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace std::string_literals;

namespace {

    Object RunFullScript(std::string const& source) {

        auto const tokens = scanTokens(source);
        if (Lox::hadError) return "Scanner error"s;
        
        auto const statements = parse(tokens);
        if (Lox::hadError) return "Parser error"s;

        auto const locals = resolve(statements);
        if (Lox::hadError) return "Resolver error"s;

        auto const result = interpret(statements, locals);
        if (Lox::hadError) return "Interpreter error"s;

        return result;
    }

    TEST_CASE("Can run a trivial script") {
        REQUIRE(RunFullScript("") == Object());
    }

    TEST_CASE("Result of last statement is returned.") {
        REQUIRE(RunFullScript("5.0;4.0;3.0;") == 3.0);
    }

    TEST_CASE("Can have global variables.") {
        REQUIRE(RunFullScript("var varTest;\nvarTest;") == Object());
    }

    TEST_CASE("Can assign to global variables.") {
        REQUIRE(RunFullScript("var varTest;\nvarTest = true;\nvarTest;") == true);
    }

    TEST_CASE("Can have if statements.") {
        REQUIRE(RunFullScript("if (true) {} else {}") == Object());
    }

    TEST_CASE("Can have for loops.") {
        REQUIRE(RunFullScript("var i; for (i=0; i != 10; i=i+1){}\ni;") == Object(10.0));
    }

    TEST_CASE("Fibonacci") {

        auto const fib = "\
fun fib(n) {\
    if (n == 0) return 0;\
    else if (n == 1) return 1;\
    return fib(n - 2) + fib(n - 1);\
}"s;

        REQUIRE(RunFullScript(fib + "fib(0);"s) == Object(0.0));
        REQUIRE(RunFullScript(fib + "fib(1);"s) == Object(1.0));
        REQUIRE(RunFullScript(fib + "fib(2);"s) == Object(1.0));
        REQUIRE(RunFullScript(fib + "fib(3);"s) == Object(2.0));
        REQUIRE(RunFullScript(fib + "fib(4);"s) == Object(3.0));
        REQUIRE(RunFullScript(fib + "fib(5);"s) == Object(5.0));
        REQUIRE(RunFullScript(fib + "fib(6);"s) == Object(8.0));
        REQUIRE(RunFullScript(fib + "fib(7);"s) == Object(13.0));
        REQUIRE(RunFullScript(fib + "fib(8);"s) == Object(21.0));
        REQUIRE(RunFullScript(fib + "fib(9);"s) == Object(34.0));
        REQUIRE(RunFullScript(fib + "fib(10);"s) == Object(55.0));
    }

    TEST_CASE("Can return local function.") {
        auto const script = "\
fun makeCounter() {\
    var i = 0;\
    fun count() {\
        i = i + 1;\
        return i;\
    }\
    return count;\
}\
var counter = makeCounter();\
counter();\
counter();\
counter();";
        REQUIRE(RunFullScript(script) == Object(3.0));
    }

    TEST_CASE("Closure captures variable from correct scope.") {
        auto const script = "\
var a = \"global\";\
var ret;\
{\
    fun getA() {\
        return a;\
    }\
    var a = \"local\";\
    ret = getA();\
}\
ret;";
        REQUIRE(RunFullScript(script) == Object("global"s));
    }

    TEST_CASE("Can create class instance.") {
        REQUIRE(RunFullScript("class Test{} Test();").isLoxInstance());
    }

}
