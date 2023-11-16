#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"
#include "Lox.h"
#include "Parser.h"
#include "ExprToString.h"
#include "Interpreter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string>
#include <cassert>
#include <algorithm>

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

    void run(std::string source) {

        auto const tokens = scanTokens(source);

        if (Lox::debugEnabled) {
            std::cout << "Tokens: ";
            std::ranges::for_each(tokens, [first = true](Token const& token) mutable { std::cout << (first ? "" : ", ") << "[" << token.toString() << "]"; first = false; });
            std::cout << std::endl;
        }

        auto const expr = parse(tokens);

        if (Lox::hadError) return;

        assert(expr && "Parser returned nullptr but no error reported! This should never happen!!!");

        if (Lox::debugEnabled) {
            std::cout << "Syntax tree: ";
            std::cout << toString(*expr) << std::endl;
        }

        auto const result = interpret(*expr);
        std::cout << result.toString() << std::endl;
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
