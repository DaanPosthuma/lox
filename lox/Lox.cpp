#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"
#include "Lox.h"
#include "Parser.h"
#include "ExprToString.h"
#include "Interpreter.h"
#include "Environment.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string>
#include <cassert>
#include <algorithm>
#include <chrono>


void Lox::error(int line, std::string message) {
    report(line, "", message);
}

void Lox::report(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void Lox::error(Token const& token, std::string const& message) {
    if (token.getTokenType() == TokenType::END_OF_FILE) {
        report(token.getLine(), " at end", message);
    }
    else {
        report(token.getLine(), " at '" + token.getLexeme() + "'", message);
    }
}

bool Lox::hadError = false;
bool Lox::debugEnabled = false;

namespace {

    void run(std::string source, Environment& environment) {

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

        auto const tInterpretStart = std::chrono::high_resolution_clock::now();
        auto const result = Lox::hadError ? Object{} : interpret(statements, environment);
        auto const tInterpretEnd = std::chrono::high_resolution_clock::now();

        if (!result.isNil()) {
            std::cout << result.toString() << std::endl;
        }

        if (Lox::debugEnabled) {
            std::cout << "Scanner: " << std::chrono::duration_cast<std::chrono::microseconds>(tScanTokensEnd - tScanTokensStart) << std::endl;
            std::cout << "Parser: " << std::chrono::duration_cast<std::chrono::microseconds>(tParseEnd - tParseStart) << std::endl;
            std::cout << "Interpreter: " << std::chrono::duration_cast<std::chrono::microseconds>(tInterpretEnd - tInterpretStart) << std::endl;
        }
    }

    void runFile(std::string fileName) {
        std::ifstream t(fileName);
        std::stringstream buffer;
        buffer << t.rdbuf();
        auto environment = Environment();
        run(buffer.str(), environment);
    }

    void runPrompt() {
        auto environment = Environment();
        while (true) {
            std::cout << "> ";
            std::string line;
            getline(std::cin, line);
            if (line == "exit" || line == "q") return;
            run(line, environment);
            Lox::hadError = false;
        }
    }
}

int main(int argc, char* argv[])
{
    std::cout << std::boolalpha;

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
