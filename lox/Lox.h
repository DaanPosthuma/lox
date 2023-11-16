#pragma once

#include <string>

class Lox {
public:
    static void error(int line, std::string message);
    static bool hadError() { return hadError_; }
    static void report(int line, std::string where, std::string message);
    static void error(Token const& token, std::string const& message);
    static void clearError();
private:
    
    static bool hadError_;
};
