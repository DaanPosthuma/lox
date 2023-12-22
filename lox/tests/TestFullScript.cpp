#include "Scanner.h"
#include "Parser.h"
#include "Resolver.h"
#include "Interpreter.h"
#include "Object.h"
#include "Lox.h"
#include "Token.h"
#include "LogListener.h"
#include "TestGuard.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace std::string_literals;

namespace {

    Object RunWitoutGuard(std::string const& source) {

        assert(!Lox::hadError);

        auto const tokens = scanTokens(source);
        if (Lox::hadError) return "Scanner error"s;

        auto const statements = parse(tokens);
        if (Lox::hadError) return "Parser error"s;

        resolve(statements);
        if (Lox::hadError) return "Resolver error"s;

        auto const result = interpret(statements);
        if (Lox::hadError) return "Interpreter error"s;

        return result;
    }

    Object RunWithGuard(std::string const& source) {
        TestGuard guard;
        return RunWitoutGuard(source);
    }

    TEST_CASE("Can run a trivial script") {
        REQUIRE(RunWithGuard("") == Object());
    }

    TEST_CASE("Result of last statement is returned.") {
        REQUIRE(RunWithGuard("5.0;4.0;3.0;") == 3.0);
    }

    TEST_CASE("Can have global variables.") {
        REQUIRE(RunWithGuard("var varTest;\nvarTest;") == Object());
    }

    TEST_CASE("Can assign to global variables.") {
        REQUIRE(RunWithGuard("var varTest;\nvarTest = true;\nvarTest;") == true);
    }

    TEST_CASE("Can have if statements.") {
        REQUIRE(RunWithGuard("if (true) {} else {}") == Object());
    }

    TEST_CASE("Can have for loops.") {
        LogListener listener;
        REQUIRE(RunWithGuard("for (var i=0; i != 10; i=i+1){ log(i); }") == Object());
        REQUIRE(listener.history() == std::vector<Object>{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0});
    }

    TEST_CASE("Fibonacci") {

        auto const fib = "\
fun fib(n) {\
    if (n == 0) return 0;\
    else if (n == 1) return 1;\
    return fib(n - 2) + fib(n - 1);\
}"s;

        REQUIRE(RunWithGuard(fib + "fib(0);"s) == 0.0);
        REQUIRE(RunWithGuard(fib + "fib(1);"s) == 1.0);
        REQUIRE(RunWithGuard(fib + "fib(2);"s) == 1.0);
        REQUIRE(RunWithGuard(fib + "fib(3);"s) == 2.0);
        REQUIRE(RunWithGuard(fib + "fib(4);"s) == 3.0);
        REQUIRE(RunWithGuard(fib + "fib(5);"s) == 5.0);
        REQUIRE(RunWithGuard(fib + "fib(6);"s) == 8.0);
        REQUIRE(RunWithGuard(fib + "fib(7);"s) == 13.0);
        REQUIRE(RunWithGuard(fib + "fib(8);"s) == 21.0);
        REQUIRE(RunWithGuard(fib + "fib(9);"s) == 34.0);
        REQUIRE(RunWithGuard(fib + "fib(10);"s) == 55.0);
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
        REQUIRE(RunWithGuard(script) == Object());
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
        REQUIRE(RunWithGuard(script) == Object());
        REQUIRE(listener.history() == std::vector<Object>{"global"s, "global"s});
    }

    TEST_CASE("Can create class instance.") {
        REQUIRE(RunWithGuard("class Test{} Test();").isLoxInstance());
    }

    TEST_CASE("Can set and get property on class instance.") {
        auto const script = "class Test{}\
var test = Test();\
test.property = 3;\
test.property;";
        REQUIRE(RunWithGuard(script) == Object(3.0));
    }

    TEST_CASE("Class can have methods.") {
        auto const script = "\
class Test{\
    method() {\
        return \"Method return value\";\
    }\
}\
var test = Test();\
test.method();";
        REQUIRE(RunWithGuard(script) == Object("Method return value"s));
    }

    TEST_CASE("Method can access this.") {
        auto const script = "\
class Test{\
    method() {\
        return this;\
    }\
}\
Test().method();";
        REQUIRE(RunWithGuard(script).isLoxInstance());
    }

    TEST_CASE("Class can have initializer.") {
        auto const script = "\
class Test{\
    init(n) {\
        this.n = n;\
    }\
    get() {\
        return this.n;\
    }\
}\
Test(5).get();";
        REQUIRE(RunWithGuard(script) == Object(5.0));
    }

    TEST_CASE("Can run line by line (REPL)") {
        TestGuard guard;
        LogListener listener;
        REQUIRE(RunWitoutGuard("var test = 3;") == Object());
        REQUIRE(RunWitoutGuard("log(test);") == Object());
        REQUIRE(RunWitoutGuard("{\nvar test = 5; log(test);}") == Object());
        REQUIRE(RunWitoutGuard("log(test);") == Object());
        REQUIRE(listener.history() == std::vector<Object>{3.0, 5.0, 3.0});
    }

    TEST_CASE("Init implicitly returns this.") {
        auto const script = "\
class Test{\
    init() {}\
}\
Test().init();";
        REQUIRE(RunWithGuard(script).isLoxInstance());

    }

    TEST_CASE("Cannot return value in intializer.") {
        auto const script = "\
class Test{\
    init() {return 3;}\
}";
        TestGuard guard;
        REQUIRE(RunWitoutGuard(script) == Object("Resolver error"s));
        REQUIRE(guard.capturedLinesCerr() == std::vector{"[line 1] Error at 'return': Can't return a value from an initializer."s});
    }

    TEST_CASE("Can return with no value in intializer.") {
        auto const script = "\
class Test{\
    init() {return;}\
}";
        TestGuard guard;
        REQUIRE(RunWitoutGuard(script) == Object());
        REQUIRE(RunWitoutGuard("Test().init();").isLoxInstance());

    }

    TEST_CASE("Class can interherit from superclass.") {
        auto const script = "\
class Super{}\
class Sub < Super{}";
        REQUIRE(RunWithGuard(script) == Object());
    }

    TEST_CASE("Class cannot interherit from itself.") {
        auto const script = "\
class Class < Class{}";
        TestGuard guard;
        REQUIRE(RunWitoutGuard(script) == Object("Resolver error"s));
        REQUIRE(guard.capturedLinesCerr() == std::vector{ "[line 1] Error at 'Class': A class can't inherit from itself."s });
    }

    TEST_CASE("Class cannot interherit from a double.") {
        auto const script = "\
var test = 3.0;\
class Class < test {}";
        TestGuard guard;
        REQUIRE(RunWitoutGuard(script) == Object("Interpreter error"s));
        REQUIRE(guard.capturedLinesCerr() == std::vector{"[line 1] Error at 'test': Superclass must be a class"s});
    }

    TEST_CASE("Can call methods from superclass.") {
        auto const script = "\
class Super {superfun(){return 1.0;}}\
class Sub < Super {}\
Sub().superfun();";
        REQUIRE(RunWithGuard(script) == Object(1.0));
    }

    TEST_CASE("Can specify super to explicitly call method on superclass.") {
        auto const script = "\
class Super {method(){return 10.0;}}\
class Sub < Super {method(){return super.method() - 10;}}\
Sub().method();";
        REQUIRE(RunWithGuard(script) == Object(0.0));
    }

    TEST_CASE("Super messy.") {
        auto const script = "\
class A {method(){print \"A method\";}}\
class B < A {method(){print \"B method\";} test(){super.method();}}\
class C < B {}\
C().method();\
C().test();\
var ctest = C().test;\
var cmethod = C().method;\
cmethod();\
ctest();";
        TestGuard guard;
        REQUIRE(RunWitoutGuard(script) == Object());
        REQUIRE(guard.capturedLinesCout() == std::vector{ "B method"s , "A method"s , "B method"s , "A method"s });

    }
}
