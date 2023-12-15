#pragma once

class Environment;
class Token;
#include <string>

class Lox {
public:
    static void error(int line, std::string message);
    static void report(int line, std::string where, std::string message);
    static void error(Token const& token, std::string const& message);

    static bool hadError;
    static bool debugEnabled;
    static Environment globals;
};
