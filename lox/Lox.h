#pragma once

#include <string>

class Lox {
public:
    static void error(int line, std::string message);

private:
    static void report(int line, std::string where, std::string message);

    static bool hadError;
};
