#pragma once

#include <string>
#include "Object.h"

enum class TokenType;


class Token {
public:
    Token(TokenType tokenType,
        std::string lexeme,
        Object literal,
        int line) : mTokenType(tokenType), mLexeme(lexeme), mLiteral(literal), mLine(line) {}
    
    std::string toString() const;
    std::string const& getLexeme() const { return mLexeme; }

private:

    TokenType mTokenType;
    std::string mLexeme;
    Object mLiteral;
    int mLine;
};
