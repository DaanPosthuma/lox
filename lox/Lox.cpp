#include "Token.h"
#include "TokenType.h"
#include "Lox.h"
#include "Environment.h"
#include "Resolver.h"
#include <iostream>
#include <string>

using namespace std::string_literals;

void Lox::error(int line, std::string message) {
    report(line, "", message);
}

void Lox::report(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void Lox::error(Token const& token, std::string const& message) {
    if (token.tokenType() == TokenType::END_OF_FILE) {
        report(token.line(), " at end", message);
    }
    else {
        report(token.line(), " at '" + token.lexeme() + "'", message);
    }
}

bool Lox::hadError = false;
bool Lox::debugEnabled = false;
Environment Lox::globals = {};
ResolvedLocals Lox::locals = {};
