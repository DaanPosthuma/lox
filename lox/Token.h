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
    TokenType getTokenType() const { return mTokenType; }
    std::string const& getLexeme() const { return mLexeme; }
    Object const& getLiteral() const { return mLiteral; }
    int getLine() const { return mLine; }

private:

    TokenType mTokenType;
    std::string mLexeme;
    Object mLiteral;
    int mLine;
};
