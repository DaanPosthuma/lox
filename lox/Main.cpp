#include "Scanner.h"
#include "Token.h"
#include "Lox.h"
#include "Parser.h"
#include "Interpreter.h"
#include "Environment.h"
#include "LoxCallable.h"
#include "Resolver.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

using namespace std::string_literals;

namespace {

    void addNativeFunctionsToGlobalEnvironment() {
        Lox::globals.define("clock", LoxCallable([](std::vector<Object> const&) {
            return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
            }, 0, "clock (native)"));

        Lox::globals.define("monkey", LoxCallable([](std::vector<Object> const&) {
            return "      __        \n w  c(..)o   (  \n  \\__(-)    __) \n      /\\   (    \n     /(_)___)   \n     w /|       \n      | \\       \n     m  m       "s;
            }, 0, "monkey (native)"));

        Lox::globals.define("readString", LoxCallable([](std::vector<Object> const&) {
            std::string s;
            std::cin >> s;
            return s;
            }, 0, "readString (native)"));

        Lox::globals.define("subString", LoxCallable([](std::vector<Object> const& arguments) {
            auto const string = static_cast<std::string>(arguments[0]);
            auto const offd = static_cast<double>(arguments[1]);
            auto const countd = static_cast<double>(arguments[2]);
            auto const off = static_cast<int>(offd);
            auto const count = static_cast<int>(countd);
            return string.substr(off, count);
            }, 3, "subString (native)"));

    }

    void run(std::string source) {

        auto const tScanTokensStart = std::chrono::high_resolution_clock::now();
        auto const tokens = scanTokens(source);
        auto const tScanTokensEnd = std::chrono::high_resolution_clock::now();

        if (Lox::debugEnabled) {
            std::cout << "Tokens: ";
            std::ranges::for_each(tokens, [first = true](Token const& token) mutable { std::cout << (first ? "" : ", ") << "[" << token.toString() << "]"; first = false; });
            std::cout << std::endl;
        }

        auto const tParseStart = std::chrono::high_resolution_clock::now();
        auto const statements = parse(tokens);
        auto const tParseEnd = std::chrono::high_resolution_clock::now();

        if (Lox::debugEnabled) {
            std::cout << "Num statements: " << statements.size() << std::endl;
        }

        auto const tResolveStart = std::chrono::high_resolution_clock::now();
        auto const locals = Lox::hadError ? ResolvedLocals() : resolve(statements);
        auto const tResolveEnd = std::chrono::high_resolution_clock::now();

        auto const tInterpretStart = std::chrono::high_resolution_clock::now();
        auto const result = Lox::hadError ? Object{} : interpret(statements, locals);
        auto const tInterpretEnd = std::chrono::high_resolution_clock::now();

        if (!result.isNil()) {
            std::cout << result.toString() << std::endl;
        }

        if (Lox::debugEnabled) {
            std::cout << "Scanner: " << std::chrono::duration_cast<std::chrono::microseconds>(tScanTokensEnd - tScanTokensStart) << std::endl;
            std::cout << "Parser: " << std::chrono::duration_cast<std::chrono::microseconds>(tParseEnd - tParseStart) << std::endl;
            std::cout << "Resolver: " << std::chrono::duration_cast<std::chrono::microseconds>(tResolveEnd - tResolveStart) << std::endl;
            std::cout << "Interpreter: " << std::chrono::duration_cast<std::chrono::microseconds>(tInterpretEnd - tInterpretStart) << std::endl;
        }
    }

    void runFile(std::string fileName) {
        std::ifstream t(fileName);
        std::stringstream buffer;
        buffer << t.rdbuf();
        run(buffer.str());
    }

    void runPrompt() {
        while (true) {
            std::cout << "> ";
            std::string line;
            getline(std::cin, line);
            if (line == "exit" || line == "q") return;
            run(line);
            Lox::hadError = false;
        }
    }
}

int main(int argc, char* argv[])
{
    std::cout << std::boolalpha;

    addNativeFunctionsToGlobalEnvironment();

    if (argc > 2) {
        std::cerr << "Usage: lox [script]" << std::endl;
        return EXIT_FAILURE;
    }
    else if (argc == 2) {
        runFile(argv[1]);
    }
    else {
        runPrompt();
    }

    return 0;
}
