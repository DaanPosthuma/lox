#pragma once

#include "Resolver.h"
#include <string>

class Environment;
class Token;

using ResolvedLocals = std::unordered_map<Expr const*, int>;

class Lox {
public:
    static void error(int line, std::string message);
    static void report(int line, std::string where, std::string message);
    static void error(Token const& token, std::string const& message);

    static bool hadError;
    static bool debugEnabled;
    static Environment globals;
    static ResolvedLocals locals;
};
