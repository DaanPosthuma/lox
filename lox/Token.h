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
    TokenType tokenType() const { return mTokenType; }
    std::string const& lexeme() const { return mLexeme; }
    Object const& literal() const { return mLiteral; }
    int line() const { return mLine; }

private:

    TokenType mTokenType;
    std::string mLexeme;
    Object mLiteral;
    int mLine;
};
