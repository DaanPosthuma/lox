#include "Scanner.h"
#include "Parser.h"
#include "Resolver.h"
#include "Interpreter.h"
#include "Object.h"
#include "Lox.h"
#include "Token.h"
#include "LogListener.h"

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
        LogListener listener;
        REQUIRE(RunFullScript("for (var i=0; i != 10; i=i+1){ log(i); }") == Object());
        REQUIRE(listener.history() == std::vector<Object>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});
    }

    TEST_CASE("Fibonacci") {

        auto const fib = "\
fun fib(n) {\
    if (n == 0) return 0;\
    else if (n == 1) return 1;\
    return fib(n - 2) + fib(n - 1);\
}"s;

        REQUIRE(RunFullScript(fib + "fib(0);"s) == 0.0);
        REQUIRE(RunFullScript(fib + "fib(1);"s) == 1.0);
        REQUIRE(RunFullScript(fib + "fib(2);"s) == 1.0);
        REQUIRE(RunFullScript(fib + "fib(3);"s) == 2.0);
        REQUIRE(RunFullScript(fib + "fib(4);"s) == 3.0);
        REQUIRE(RunFullScript(fib + "fib(5);"s) == 5.0);
        REQUIRE(RunFullScript(fib + "fib(6);"s) == 8.0);
        REQUIRE(RunFullScript(fib + "fib(7);"s) == 13.0);
        REQUIRE(RunFullScript(fib + "fib(8);"s) == 21.0);
        REQUIRE(RunFullScript(fib + "fib(9);"s) == 34.0);
        REQUIRE(RunFullScript(fib + "fib(10);"s) == 55.0);
    }

    TEST_CASE("Can return local function.") {
        LogListener listener;
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
log(counter());\
log(counter());\
log(counter());";
        REQUIRE(RunFullScript(script) == Object());
        REQUIRE(listener.history() == std::vector<Object>{1.0, 2.0, 3.0});
    }

    TEST_CASE("Closure captures variable from correct scope.") {
        auto const script = "\
var a = \"global\";\
{\
    fun getA() {\
        return a;\
    }\
    log(getA());\
    var a = \"local\";\
    log(getA());\
}";
        LogListener listener;
        REQUIRE(RunFullScript(script) == Object());
        REQUIRE(listener.history() == std::vector<Object>{"global"s, "global"s});
    }

    TEST_CASE("Can create class instance.") {
        REQUIRE(RunFullScript("class Test{} Test();").isLoxInstance());
    }

    TEST_CASE("Can set and get property on class instance.") {
        auto const script = "class Test{}\
var test = Test();\
test.property = 3;\
test.property;";
        REQUIRE(RunFullScript(script) == Object(3.0));
    }

}
