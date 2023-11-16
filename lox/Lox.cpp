#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string>
#include <cassert>

#include "Scanner.h"
#include "Token.h"
#include "TokenType.h"
#include "Lox.h"
#include "Parser.h"
#include "ExprToString.h"

void Lox::error(int line, std::string message) {
    report(line, "", message);
}

void Lox::report(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError_ = true;
}

void Lox::error(Token const& token, std::string const& message) {
    if (token.getTokenType() == TokenType::END_OF_FILE) {
        report(token.getLine(), " at end", message);
    }
    else {
        report(token.getLine(), " at '" + token.getLexeme() + "'", message);
    }
}

void Lox::clearError() { 
    hadError_ = false; 
}


bool Lox::hadError_ = false;

namespace {

    void run(std::string source) {

        Lox::clearError();

        auto scanner = Scanner(source);
        auto const& tokens = scanner.scanTokens();
        
        if (tokens.size() > 1 && next(tokens.rbegin())->getTokenType() == TokenType::PRINT_TOKENS) {
            for (auto const& token : tokens) {
                std::cout << token.toString() << std::endl;
            }
        }

        auto parser = Parser(tokens);
        auto expr = parser.parse();

        if (Lox::hadError()) return;
        
        assert(expr && "Parser returned nullptr but no error reported! This should never happen!!!");
        std::cout << toString(*expr) << std::endl;
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
