#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Object.h"

class Token;
enum class TokenType;

class Scanner {
public:
    Scanner(std::string source) : mSource(source) {}

    std::vector<Token> const& scanTokens();

private:
    
    void scanToken();

    bool isAtEnd() const;
    char advance();

    void addToken(TokenType type, Object literal = {});
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    void string();
    void number();
    void identifier();

    std::string mSource;
    std::vector<Token> mTokens;

    int mStart = 0;
    int mCurrent = 0;
    int mLine = 1;

    static std::unordered_map<std::string, TokenType> keywords;
};
